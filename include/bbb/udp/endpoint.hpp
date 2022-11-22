/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/udp/endpoint.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_udp_endpoint_hpp
#define bbb_udp_endpoint_hpp

#include <string>

#include <cstddef>
#include <cstdint>

namespace bbb {
    namespace udp {
        struct endpoint {
            std::string host;
            std::uint16_t port;
            bool operator<(const endpoint &rhs) const {
                if(host == rhs.host) return port < rhs.port;
                return host < rhs.host;
            }
        }; // struct endpoint
    }; // namespace udp
}; // namespace bbb

#endif // bbb_udp_endpoint_hpp
