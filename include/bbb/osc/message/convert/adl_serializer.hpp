/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/message/convert/adl_serializer.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_message_convert_adl_serializer_hpp
#define bbb_osc_message_convert_adl_serializer_hpp

#include <bbb/osc/message.hpp>

#include <type_traits>

namespace bbb {
    namespace osc {
        template <typename value_type, typename>
        struct adl_serializer {
            template <
                typename osc_message,
                typename target_type = value_type
            >
            static auto from_osc(const osc_message &m,
                                 value_type &v,
                                 std::size_t offset = 0)
                -> decltype(::bbb::osc::from_osc(m, v))
            {
                return ::bbb::osc::from_osc(m, v);
            }
            
            template <
                typename osc_message,
                typename target_type = value_type
            >
            static auto to_osc(osc_message &m,
                               const value_type &v,
                               std::size_t offset = 0)
                -> decltype(::bbb::osc::from_osc(m, v, offset))
            {
                return ::bbb::osc::from_osc(m, v, offset);
            }
        };
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_message_convert_adl_serializer_hpp */
