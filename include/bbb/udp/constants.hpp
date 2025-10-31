/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/udp/constants.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_udp_constants_hpp
#define bbb_udp_constants_hpp

#include <cstddef>
#include <cstdint>

namespace bbb {
    namespace udp {
        static constexpr std::size_t buf_size{65536};
    }; // namespace udp
}; // namespace bbb

#endif /* bbb_udp_constants_hpp */
