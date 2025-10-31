/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/udp/sender.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_udp_sender_hpp
#define bbb_udp_sender_hpp

#include <bbb/udp/endpoint.hpp>
#include <bbb/udp/socket.hpp>

#include <system_error>

namespace bbb {
    namespace udp {
        struct sender : public socket {
            void setup(const std::string &host,
                       std::uint16_t port)
            {
                sock.open(asio::ip::udp::v4());
                
                /// TODO: research more correct way to resolve "localhost"
                auto ip = host == "localhost"
                    ? asio::ip::make_address("127.0.0.1")
                    : asio::ip::make_address(host);
                target = asio::ip::udp::endpoint(ip, port);
                asio::socket_base::broadcast option{true};
                sock.set_option(option);
            }
            
            bool send(const void *buffer, std::size_t size) {
                return send_buffer(asio::buffer(buffer, size));
            }

            bool send(const std::vector<std::uint8_t> &buffer) {
                return send_buffer(asio::buffer(buffer));
            }

            bool send(const std::string &buffer) {
                return send_buffer(asio::buffer(buffer));
            }
        protected:
            asio::ip::udp::endpoint target;
            template <typename buffer_t>
            bool send_buffer(buffer_t &&buffer) {
                asio::error_code err;
                sock.send_to(std::forward<buffer_t>(buffer),
                             target,
                             0,
                             err);
                if(!err) return true;
                std::cerr << "error on bbb::udp::sender::send: error code is " << err.value() << std::endl;
                return false;
            }
        }; // struct sender
    }; // namespace udp
}; // namespace bbb


#endif /* bbb_udp_sender_hpp */
