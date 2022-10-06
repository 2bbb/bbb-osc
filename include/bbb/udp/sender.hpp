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

#include <bbb/udp/socket.hpp>

namespace bbb {
    namespace udp {
        struct sender : public socket {
            struct endpoint {
                std::string host;
                std::uint16_t port;
                bool operator<(const endpoint &rhs) const {
                    if(host == rhs.host) return port < rhs.port;
                    return host < rhs.host;
                }
            };


            void setup(const std::string &host,
                       std::uint16_t port)
            {
                sock.open(boost::asio::ip::udp::v4());
                
                /// TODO: research more correct way to resolve "localhost"
                auto ip = host == "localhost"
                    ? boost::asio::ip::address::from_string("127.0.0.1")
                    : boost::asio::ip::address::from_string(host);
                target = boost::asio::ip::udp::endpoint(ip, port);
                boost::asio::socket_base::broadcast option{true};
                sock.set_option(option);
            }
            
            bool send(void *buffer, std::size_t size) {
                return send_buffer(boost::asio::buffer(buffer, size));
            }

            bool send(const std::vector<std::uint8_t> &buffer) {
                return send_buffer(boost::asio::buffer(buffer));
            }

            bool send(const std::string &buffer) {
                return send_buffer(boost::asio::buffer(buffer));
            }
        protected:
            boost::asio::ip::udp::endpoint target;
            template <typename buffer_t>
            bool send_buffer(const buffer_t &buffer) {
                boost::system::error_code err;
                sock.send_to(buffer,
                             target,
                             0,
                             err);
                if(err == boost::system::errc::errc_t::success) return true;
                std::cerr << "error on bbb::udp::sender::send: error code is " << err.value() << std::endl;
                return false;
            }
        }; // struct sender
    }; // namespace udp
}; // namespace bbb


#endif /* bbb_udp_sender_hpp */
