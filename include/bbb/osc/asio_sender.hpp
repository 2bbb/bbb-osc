/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/asio_sender.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_asio_sender_hpp
#define bbb_osc_asio_sender_hpp

#include <bbb/osc/custom_sender.hpp>
#include <bbb/udp/sender.hpp>

namespace bbb {
    namespace osc {
        namespace detail {
            struct asio_udp_sender
                : public bbb::osc::udp_sender_interface
                , public bbb::udp::sender
            {
                using base_sender = bbb::udp::sender;
                asio_udp_sender()
                : bbb::udp::sender()
                {}

                virtual ~asio_udp_sender()
                {}
                
                virtual void setup(const std::string &ip,
                                   std::uint16_t port) override
                {
                    this->base_sender::setup(ip, port);
                }
                
                virtual void send_buffer(const void *buffer,
                                         std::size_t length) override
                {
                    this->base_sender::send(buffer, length);
                }
            }; // struct asio_udp_sender
        }; // namespace detail

        using asio_sender = bbb::osc::custom_sender<bbb::osc::detail::asio_udp_sender>;
    }; // namespace osc
}; // namespace bbb


#endif /* bbb_osc_asio_receiver_hpp */
