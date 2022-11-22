/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/custom_sender.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_custom_sender_hpp
#define bbb_osc_custom_sender_hpp

#include <bbb/osc/message.hpp>
#include <bbb/osc/common.hpp>

#include <bbb/udp/constants.hpp>
#include <bbb/udp/endpoint.hpp>

namespace bbb {
    namespace osc {
        struct udp_sender_interface {
            udp_sender_interface() {};
            virtual ~udp_sender_interface() {};
            virtual void setup(const std::string &address, std::uint16_t port) = 0;
        protected:
            virtual void send_buffer(const void *buffer, std::size_t length) = 0;
        };

        template <
            typename udp_sender,
            typename = typename std::enable_if<
                std::is_base_of<udp_sender_interface, udp_sender>::value
                && std::is_default_constructible<udp_sender>::value
            >::type
        >
        struct custom_sender : public udp_sender {
        public:
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
                this->udp_sender::send_buffer(buffer, packet_size);
            }

            struct manager final {
                static manager &shared() {
                    static manager _;
                    return _;
                }
                
                std::map<bbb::udp::endpoint, std::shared_ptr<custom_sender>> senders;
                
                template <typename derived_sender = custom_sender>
                auto get(const std::string &host, std::uint16_t port)
                    -> typename std::enable_if<
                        std::is_base_of<custom_sender, derived_sender>::value,
                        std::shared_ptr<derived_sender>
                    >::type
                {
                    bbb::udp::endpoint endpoint{host, port};
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
                friend class custom_sender;
            }; // struct manager

            template <typename derived_sender = custom_sender>
            static auto get(const std::string &host, std::uint16_t port)
                -> typename std::enable_if<
                    std::is_base_of<custom_sender, derived_sender>::value,
                    std::shared_ptr<derived_sender>
                >::type
            {
                return manager::shared().template get<derived_sender>(host, port);
            }
        }; // struct custom_sender
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_custom_sender_hpp */
