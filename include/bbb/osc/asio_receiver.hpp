/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/asio_receiver.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_asio_receiver_hpp
#define bbb_osc_asio_receiver_hpp

#include <bbb/osc/custom_receiver.hpp>
#include <bbb/udp/receiver.hpp>

namespace bbb {
    namespace osc {
        namespace detail {
            struct asio_udp_receiver
                : public bbb::osc::udp_receiver_interface
                , public bbb::udp::receiver
            {
                asio_udp_receiver()
                : bbb::udp::receiver() {}

                virtual ~asio_udp_receiver()
                {}
                
                virtual bool setup(std::uint16_t port) override {
                    auto success = this->bbb::udp::receiver::setup(port);
                    return success;
                }
                
            protected:
                virtual void receive(const boost::system::error_code &error_code,
                                    std::array<char, bbb::udp::buf_size> &buf,
                                    std::size_t len) override
                { process_buffer(buf.data(), len); }
            }; // struct asio_udp_receiver
        }; // namespace detail

        using asio_receiver = bbb::osc::custom_receiver<bbb::osc::detail::asio_udp_receiver>;
    }; // namespace osc
}; // namespace bbb


#endif /* bbb_osc_asio_receiver_hpp */
