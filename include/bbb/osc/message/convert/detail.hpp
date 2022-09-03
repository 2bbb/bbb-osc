/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/message/convert/detail.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_message_convert_detail_hpp
#define bbb_osc_message_convert_detail_hpp

namespace bbb {
    namespace osc {
        namespace convert {
            namespace detail {
                template <typename type>
                struct static_const
                { static constexpr type value{}; };

                template <typename type>
                constexpr type static_const<type>::value;
            }; // namespace detail
        }; // namespace convert
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_message_convert_detail_hpp */
