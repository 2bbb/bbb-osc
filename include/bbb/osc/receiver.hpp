/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/receiver.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_receiver_hpp
#define bbb_osc_receiver_hpp

#include <bbb/osc/message.hpp>
#include <bbb/osc/common.hpp>

#include <bbb/udp/receiver.hpp>

#include <bbb/thread/threaded_queue.hpp>

#include <array>
#include <unordered_map>
#include <memory>
#include <map>

namespace bbb {
    namespace osc {
        struct receiver : public bbb::udp::receiver {
            using callback_t = std::function<void(bbb::osc::message &mess)>;
            using port_and_local_ip = std::pair<std::uint16_t, std::string>;
            receiver()
            : bbb::udp::receiver() {}
            
            virtual ~receiver()
            {}
            
            using bbb::udp::receiver::setup;
            
            static void update(std::uint16_t port,
                               std::string local_ip = "0.0.0.0")
            {
                get(port, local_ip)->update(port);
            }

            void update() {
                before_update();
                
                leaked_messages.clear();
                bbb::osc::message mess;
                while(!queued_messages.empty()) {
                    if(queued_messages.receive(mess)) {
                        auto it = callbacks.find(mess.address);
                        if(it != callbacks.end()) {
                            for(std::size_t i = 0, num = callbacks.count(mess.address); i < num; ++it, i++) {
                                it->second(mess);
                            }
                        } else {
                            if(!thread_processes.count(mess.address)) {
                                leaked_messages.push_back(std::move(mess));
                            }
                        }
                    }
                }
                for(auto process : thread_processes) process.second->call();
                
                after_update();
            }
            
            void on_before_update(std::function<void()> callback) {
                before_update_callbacks.push_back(callback);
            }
            
            void on_after_update(std::function<void()> callback) {
                after_update_callbacks.push_back(callback);
            }

            void bind(const std::string &bind_address,
                      callback_t callback)
            {
                callbacks.insert(std::make_pair(bind_address, callback));
            }
            
            void bind(const std::string &bind_address,
                      std::function<void()> no_arg_callback)
            {
                callback_t callback = [no_arg_callback](const bbb::osc::message &) {
                    no_arg_callback();
                };
                callbacks.insert(std::make_pair(bind_address, callback));
            }

            using iterator = std::vector<bbb::osc::message>::iterator;
            using const_iterator = std::vector<bbb::osc::message>::const_iterator;
            
            iterator begin() { return leaked_messages.begin(); }
            iterator end() { return leaked_messages.end(); }
            const_iterator begin() const { return leaked_messages.cbegin(); }
            const_iterator end() const { return leaked_messages.cend(); }
            const_iterator cbegin() const { return leaked_messages.cbegin(); }
            const_iterator cend() const { return leaked_messages.cend(); }
            
            template <typename Prepared>
            void bind_with_threading(const std::string &bind_address,
                                     thread_process<Prepared> thread_process,
                                     prepared_callback<Prepared> callback)
            {
                thread_processes.insert(std::make_pair(bind_address, makeThreadedCallback(thread_process, callback)));
            }
            
            template <typename ThreadProcess, typename Callback>
            void bind_with_threading(const std::string &bind_address,
                                     ThreadProcess tp,
                                     Callback cb)
            {
                static_assert(is_argument_message<ThreadProcess>(), "argument of ThreadProcess is not bbb::osc::message.");
                static_assert(has_call_operator<ThreadProcess>::value, "ThreadProcess is not function.");
                static_assert(has_call_operator<Callback>::value, "Callback is not function.");
                static_assert(composable<ThreadProcess, Callback>(), "result of ThreadProcess is not argument of Callback.");
                
                bind_with_threading(bind_address,
                                    static_cast<typename function_traits<ThreadProcess>::function_type>(tp),
                                    static_cast<typename function_traits<Callback>::function_type>(cb));
            }
            
        protected:
            bbb::threaded_queue<bbb::osc::message> queued_messages;
            std::vector<bbb::osc::message> leaked_messages;
            std::unordered_multimap<std::string, callback_t> callbacks;
            std::unordered_multimap<std::string, abstract_threaded_callback::ref> thread_processes;
            
            std::vector<std::function<void()>> before_update_callbacks;
            std::vector<std::function<void()>> after_update_callbacks;

            void before_update() {
                for(const auto &cb : before_update_callbacks) {
                    cb();
                }
            }
            void after_update() {
                for(const auto &cb : after_update_callbacks) {
                    cb();
                }
            }

            virtual void receive(const boost::system::error_code &error_code,
                                 std::array<char, bbb::udp::buf_size> &buf,
                                 std::size_t len,
                                 boost::asio::ip::udp::endpoint remote_endpoint) override
            {
                handle(OSCPP::Server::Packet(buf.data(), len),
                       binded_local_ip,
                       binded_port,
                       remote_endpoint.address().to_string(),
                       remote_endpoint.port());
            }
            
            void handle(const OSCPP::Server::Packet &packet,
                        const std::string &waiting_ip,
                        std::uint16_t waiting_port,
                        const std::string &received_host,
                        std::uint16_t received_port)
            {
                if(packet.isBundle()) {
                    // Convert to bundle
                    OSCPP::Server::Bundle bundle(packet);
                    // Get packet stream
                    OSCPP::Server::PacketStream packets(bundle.packets());
                    
                    while(!packets.atEnd()) {
                        handle(packets.next(),
                               waiting_ip,
                               waiting_port,
                               received_host,
                               received_port);
                    }
                } else {
                    OSCPP::Server::Message msg(packet);
                    
                    std::string address(msg.address());
                    OSCPP::Server::ArgStream args(msg.args());
                    
                    bbb::osc::message mess(address);
                    mess.binded_ip = waiting_ip;
                    mess.waiting_port = waiting_port;
                    mess.received_host = received_host;
                    mess.received_port = received_port;

                    while(!args.atEnd()) {
                        Tag tag = static_cast<Tag>(args.tag());
                        switch(tag) {
                            case OSCPP::Tag::True:
                            case OSCPP::Tag::False:
                            case OSCPP::Tag::IMPULSE:
                            case OSCPP::Tag::NIL:
                                mess.push(tag);
                                break;
                            case OSCPP::Tag::Char:
                                mess.push(tag, args.int8());
                                break;
                            case OSCPP::Tag::Int32:
                                mess.push(tag, args.int32());
                                break;
                            case OSCPP::Tag::Int64:
                                mess.push(tag, args.int64());
                                break;
                            case OSCPP::Tag::Float:
                                mess.push(tag, args.float32());
                                break;
                            case OSCPP::Tag::Double:
                                mess.push(tag, args.float64());
                                break;
                            case OSCPP::Tag::String:
                            case OSCPP::Tag::Symbol:
                                mess.push(tag, args.string());
                                break;
                            case OSCPP::Tag::Midi4:
                                mess.push(tag, args.int32());
                                break;
                            case OSCPP::Tag::RGBA:
                                mess.push(tag, args.int32());
                                break;
                            case OSCPP::Tag::Blob: {
                                //TODO: implement
                                auto &&blob = args.blob();
                                mess.push(tag, blob.data(), blob.size());
                                break;
                            }
                            case OSCPP::Tag::Timetag:
                                //TODO: implement
//
                                break;
                        }
                    }
                    
                    queued_messages.send(std::move(mess));
                    
                    auto range = thread_processes.equal_range(address);
                    for(auto it = range.first; it != range.second; ++it) {
                        it->second->process(mess);
                    }
                }
            }
            
        public:
            struct manager final {
                static manager &shared() {
                    static manager _;
                    return _;
                }
                
                std::map<
                    port_and_local_ip,
                    std::shared_ptr<bbb::osc::receiver>
                > receivers;
                mutable std::mutex receivers_lock;

                bool find(std::uint16_t port, std::string host = "0.0.0.0") const {
                    port_and_local_ip key{port, host};
                    auto lock = std::lock_guard(receivers_lock);
                    auto it = receivers.find(key);
                    return it != receivers.end();
                }

                template <typename derived_receiver = bbb::osc::receiver>
                auto get(std::uint16_t port,
                         std::string local_ip = "0.0.0.0")
                    -> typename std::enable_if<
                        std::is_base_of<bbb::osc::receiver, derived_receiver>::value,
                        std::shared_ptr<derived_receiver>
                    >::type
                {
                    auto key = port_and_local_ip{port, local_ip};
                    {
                        auto lock = std::lock_guard(receivers_lock);
                        auto it = receivers.find(key);
                        if(it != receivers.end()) {
                            return std::dynamic_pointer_cast<derived_receiver>(it->second);
                        }
                    }
                    
                    auto receiver = std::make_shared<derived_receiver>();
                    auto res = receiver->setup(port, local_ip);
                    if(!res) {
                        std::cerr << "bbb::osc::receiver::manager::setup: failed to setup " << port << std::endl;
                        return std::shared_ptr<derived_receiver>();
                    }
                    {
                        auto lock = std::lock_guard(receivers_lock);
                        receivers.insert(std::make_pair(key, receiver));
                    }
                    return receiver;
                }
                
                inline void bind(std::uint16_t port,
                                 const std::string &address,
                                 std::function<void()> callback)
                {
                    bind(port, "0.0.0.0", address, callback);
                }
                inline void bind(std::uint16_t port,
                                 std::string local_ip,
                                 const std::string &address,
                                 std::function<void()> callback)
                {
                    if(find(port, local_ip)) {
                        auto lock = std::lock_guard(receivers_lock);
                        get(port, local_ip)->bind(address, callback);
                    }
                }
                
                inline void bind(std::uint16_t port,
                                 const std::string &address,
                                 callback_t callback)
                {
                    bind(port, "0.0.0.0", address, callback);
                }
                inline void bind(std::uint16_t port,
                                 std::string local_ip,
                                 const std::string &address,
                                 callback_t callback)
                {
                    if(find(port, local_ip)) {
                        auto lock = std::lock_guard(receivers_lock);
                        get(port, local_ip)->bind(address, callback);
                    }
                }

                inline void update(std::uint16_t port,
                                   std::string local_ip = "0.0.0.0")
                {
                    if(find(port, local_ip)) {
                        auto lock = std::lock_guard(receivers_lock);
                        get(port, local_ip)->update();
                    }
                }
                
                inline void update() {
                    auto lock = std::lock_guard(receivers_lock);
                    for(auto pair : receivers) {
                        pair.second->update();
                    }
                }
                
                inline void close(std::uint16_t port,
                                  std::string local_ip = "0.0.0.0")
                {
                    if(find(port, local_ip)) {
                        get(port, local_ip)->close();
                        auto lock = std::lock_guard(receivers_lock);
                        receivers.erase(port_and_local_ip{port, local_ip});
                    }
                }

                inline void close() {
                    for(auto pair : receivers) {
                        pair.second->close();
                    }
                }
            private:
                manager() {};
                manager(const manager &) = delete;;
                manager &operator=(const manager &) = delete;
                friend struct bbb::osc::receiver;
            }; // struct manager

            template <typename derived_receiver = bbb::osc::receiver>
            static auto get(std::uint16_t port,
                            std::string local_ip = "0.0.0.0")
                -> typename std::enable_if<
                    std::is_base_of<bbb::osc::receiver, derived_receiver>::value,
                    std::shared_ptr<derived_receiver>
                >::type
            {
                return manager::shared().get<derived_receiver>(port, local_ip);
            }
        }; // struct receiver
    }; // namespace osc
}; // namespace bbb


#endif /* bbb_osc_receiver_hpp */
