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

#define BOOST_DATE_TIME_NO_LIB 
#define BOOST_REGEX_NO_LIB
#define BOOST_ERROR_CODE_HEADER_ONLY
#define BOOST_SYSTEM_NO_LIB

#include <cstddef>
#include <cstdint>

namespace bbb {
    namespace udp {
        static constexpr std::size_t buf_size{65536};
    }; // namespace udp
}; // namespace bbb

#endif /* bbb_udp_constants_hpp */
