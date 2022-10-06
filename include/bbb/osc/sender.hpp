/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/sender.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_sender_hpp
#define bbb_osc_sender_hpp

#include <bbb/osc/message.hpp>
#include <bbb/osc/common.hpp>

#include <bbb/udp/sender.hpp>

namespace bbb {
    namespace osc {
        class sender : public bbb::udp::sender {
        public:
            using bbb::udp::sender::setup;
            using endpoint = bbb::udp::sender::endpoint;

            template <typename ... arguments>
            void send(const std::string &address,
                      arguments && ... args)
            {
                send(bbb::osc::message(address,
                                       std::forward<arguments>(args) ...));
            }
            
            void send(const std::string &address,
                      bbb::osc::message mess)
            {
                mess.address = address;
                send(mess);
            }
            
            void send(const bbb::osc::message &mess)
            {
                std::uint8_t buffer[bbb::udp::buf_size]{0};
                auto packet_size = mess.packet(buffer, bbb::udp::buf_size);
                this->udp::sender::send(buffer, packet_size);
            }

            struct manager final {
                static manager &shared() {
                    static manager _;
                    return _;
                }
                
                std::map<bbb::osc::sender::endpoint, std::shared_ptr<bbb::osc::sender>> senders;
                
                template <typename derived_sender = bbb::osc::sender>
                auto get(const std::string &host, std::uint16_t port)
                    -> typename std::enable_if<
                        std::is_base_of<bbb::osc::sender, derived_sender>::value,
                        std::shared_ptr<derived_sender>
                    >::type
                {
                    bbb::osc::sender::endpoint endpoint{host, port};
                    auto it = senders.find(endpoint);
                    if(it != senders.end()) {
                        return std::dynamic_pointer_cast<derived_sender>(it->second);
                    }
                    
                    auto sender = std::make_shared<derived_sender>();
                    senders.insert(std::make_pair(std::move(endpoint), sender));
                    return sender;
                }
                
                template <typename ... arguments>
                inline void send(const std::string &host,
                                 std::uint16_t port,
                                 const std::string &address,
                                 arguments && ... args)
                {
                    get(host, port)->send(bbb::osc::message(address,
                                          std::forward<arguments>(args) ...));
                }
                
                inline void send(const std::string &host,
                                 std::uint16_t port,
                                 const std::string &address,
                                 bbb::osc::message mess)
                {
                    get(host, port)->send(mess);
                }
                
                inline void send(const std::string &host,
                                 std::uint16_t port,
                                 const bbb::osc::message &mess)
                {
                    get(host, port)->send(mess);
                }

            private:
                manager() {};
                manager(const manager &) = delete;;
                manager &operator=(const manager &) = delete;
                friend class bbb::osc::sender;
            }; // struct manager

            template <typename derived_sender = bbb::osc::sender>
            static auto get(const std::string &host, std::uint16_t port)
                -> typename std::enable_if<
                    std::is_base_of<bbb::osc::sender, derived_sender>::value,
                    std::shared_ptr<derived_sender>
                >::type
            {
                return manager::shared().get<derived_sender>(host, port);
            }
        }; // struct sender
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_sender_hpp */
