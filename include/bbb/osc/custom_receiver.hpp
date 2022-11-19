/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/custom_receiver.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_custom_receiver_hpp
#define bbb_osc_custom_receiver_hpp

#include <bbb/osc/message.hpp>
#include <bbb/osc/common.hpp>

#include <bbb/thread/threaded_queue.hpp>

#include <array>
#include <unordered_map>
#include <memory>
#include <map>

namespace bbb {
    namespace osc {
        struct udp_receiver_interface {
            udp_receiver_interface() {};

            virtual bool setup(std::uint16_t port) = 0;

            // will implement on bbb::osc::custom_receiver
            virtual void process_buffer(const char * const data, std::size_t length) {};
        };

        template <
            typename udp_receiver,
            typename = typename std::enable_if<
                std::is_base_of<udp_receiver_interface, udp_receiver>::value
                && std::is_default_constructible<udp_receiver>::value
            >::type
        >
        struct custom_receiver : public udp_receiver {
            using callback_t = std::function<void(bbb::osc::message &mess)>;
            
            custom_receiver()
            : udp_receiver() {}
            
            virtual ~custom_receiver()
            {}
            
            using udp_receiver::setup;
            
            static void update(std::uint16_t port) {
                get(port)->update(port);
            }

            void update() {
                before_update();
                
                leaked_messages.clear();
                bbb::osc::message mess;
                while(!queued_messages.empty()) {
                    if(queued_messages.receive(mess)) {
                        std::size_t num = callbacks.count(mess.address);
                        auto it = callbacks.find(mess.address);
                        if(it != callbacks.end()) for(std::size_t i = 0, num = callbacks.count(mess.address); i < num; ++it, i++) {
                            it->second(mess);
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

            void bind(const std::string &bindAddress,
                      callback_t callback)
            {
                callbacks.insert(std::make_pair(bindAddress, callback));
            }
            
            void bind(const std::string &bindAddress,
                      std::function<void()> no_arg_callback)
            {
                callback_t callback = [no_arg_callback](const bbb::osc::message &) {
                    no_arg_callback();
                };
                callbacks.insert(std::make_pair(bindAddress, callback));
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
            void bind_with_threading(const std::string &bindAddress,
                                     thread_process<Prepared> thread_process,
                                     prepared_callback<Prepared> callback)
            {
                thread_processes.insert(std::make_pair(bindAddress, makeThreadedCallback(thread_process, callback)));
            }
            
            template <typename ThreadProcess, typename Callback>
            void bind_with_threading(const std::string &bindAddress,
                                     ThreadProcess tp,
                                     Callback cb)
            {
                static_assert(is_argument_message<ThreadProcess>(), "argument of ThreadProcess is not bbb::osc::message.");
                static_assert(has_call_operator<ThreadProcess>::value, "ThreadProcess is not function.");
                static_assert(has_call_operator<Callback>::value, "Callback is not function.");
                static_assert(composable<ThreadProcess, Callback>(), "result of ThreadProcess is not argument of Callback.");
                
                bind_with_threading(bindAddress,
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

            virtual void process_buffer(const char * const data, std::size_t length) override {
                handle(OSCPP::Server::Packet(data, length));
            }

            void handle(const OSCPP::Server::Packet &packet) {
                if(packet.isBundle()) {
                    // Convert to bundle
                    OSCPP::Server::Bundle bundle(packet);
                    // Get packet stream
                    OSCPP::Server::PacketStream packets(bundle.packets());
                    
                    while(!packets.atEnd()) {
                        handle(packets.next());
                    }
                } else {
                    OSCPP::Server::Message msg(packet);
                    
                    std::string address(msg.address());
                    OSCPP::Server::ArgStream args(msg.args());
                    
                    bbb::osc::message mess(address);
                    
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
                                mess.push(tag, args.string());
                                break;
                            default:
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
                
                std::map<std::uint16_t, std::shared_ptr<custom_receiver>> receivers;
                
                template <typename derived_receiver = custom_receiver>
                auto get(std::uint16_t port)
                    -> typename std::enable_if<
                        std::is_base_of<custom_receiver, derived_receiver>::value,
                        std::shared_ptr<derived_receiver>
                    >::type
                {
                    auto it = receivers.find(port);
                    if(it != receivers.end()) {
                        return std::dynamic_pointer_cast<derived_receiver>(it->second);
                    }
                    
                    auto receiver = std::make_shared<derived_receiver>();
                    auto res = receiver->setup(port);
                    if(!res) {
                        std::cerr << "bbb::osc::custom_receiver::manager::setup: failed to setup " << port << std::endl;
                        return std::shared_ptr<derived_receiver>();
                    }
                    receivers.insert(std::make_pair(port, receiver));
                    return receiver;
                }
                
                inline void bind(std::uint16_t port,
                                 const std::string &address,
                                 std::function<void()> callback)
                {
                    get(port)->bind(address, callback);
                }
                inline void bind(std::uint16_t port,
                                 const std::string &address,
                                 callback_t callback)
                {
                    get(port)->bind(address, callback);
                }
                
                inline void update(std::uint16_t port) {
                    get(port)->update();
                }
                
                inline void update() {
                    for(auto pair : receivers) {
                        pair.second->update();
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
                friend class custom_receiver;
            }; // struct manager

            template <typename derived_receiver = custom_receiver>
            static auto get(std::uint16_t port)
                -> typename std::enable_if<
                    std::is_base_of<custom_receiver, derived_receiver>::value,
                    std::shared_ptr<derived_receiver>
                >::type
            {
                return manager::shared().template get<derived_receiver>(port);
            }
        }; // struct receiver
    }; // namespace osc
}; // namespace bbb


#endif /* bbb_osc_custom_receiver_hpp */
