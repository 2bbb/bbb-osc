/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/udp/socket.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_udp_socket_hpp
#define bbb_udp_socket_hpp

#include <bbb/udp/constants.hpp>
#include <bbb/core.hpp>

#include <boost/asio.hpp>

namespace bbb {
    namespace udp {
        struct socket {
            socket()
            : sock(io_service) {}
        protected:
            boost::asio::io_service io_service;
            boost::asio::ip::udp::socket sock;
        }; // struct socket
    }; // namespace udp
}; // namespace bbb


#endif /* bbb_udp_socket_hpp */
