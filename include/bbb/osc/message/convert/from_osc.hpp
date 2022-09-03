/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/message/convert/from_osc.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once


#ifndef bbb_osc_message_convert_from_osc_hpp
#define bbb_osc_message_convert_from_osc_hpp

#include <bbb/osc/message/convert/detail.hpp>

#include <bbb/osc/argument.hpp>

#include <bbb/osc/common.hpp>

namespace bbb {
    namespace osc {
        namespace convert {
#define BBB_DEF_CONV_OSC(TYPE) \
            template <typename osc_message> \
            inline std::size_t from_osc(const osc_message &m, \
                                        TYPE &v, \
                                        std::size_t offset = 0) \
            { \
                v = m[offset].template get<TYPE>(); \
                return 1; \
            }
            BBB_DEF_CONV_OSC(bool);
            BBB_DEF_CONV_OSC(char);
            BBB_DEF_CONV_OSC(signed char);
            BBB_DEF_CONV_OSC(unsigned char);
            BBB_DEF_CONV_OSC(short);
            BBB_DEF_CONV_OSC(unsigned short);
            BBB_DEF_CONV_OSC(int);
            BBB_DEF_CONV_OSC(unsigned int);
            BBB_DEF_CONV_OSC(long);
            BBB_DEF_CONV_OSC(unsigned long);
            BBB_DEF_CONV_OSC(float);
            BBB_DEF_CONV_OSC(double);
#undef BBB_DEF_CONV_OSC

            struct from_osc_fn {
                template <
                    typename osc_message,
                    typename value_type
                >
                auto operator()(const osc_message &m,
                                value_type &&v,
                                std::size_t offset = 0) const
                    -> decltype(from_osc(m, std::forward<value_type>(v), offset))
                {
                    return from_osc(m, std::forward<value_type>(v), offset);
                }
            }; // struct from_osc_fn
        }; // namespace detail

        namespace {
            constexpr const auto& from_osc = bbb::osc::convert::detail::static_const<convert::from_osc_fn>::value;
        };
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_message_convert_from_osc_hpp */
