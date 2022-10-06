/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/message/convert/to_osc.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_message_convert_to_osc_hpp
#define bbb_osc_message_convert_to_osc_hpp

#include <bbb/osc/message/convert/detail.hpp>

#include <bbb/osc/argument.hpp>

#include <bbb/osc/common.hpp>

#include <bbb/core.hpp>

namespace bbb {
    namespace osc {
        namespace convert {
            template <
                typename osc_message,
                typename type,
                typename = bbb::void_t<>
            >
            struct has_to_osc
            : std::false_type {};

            template <
                typename osc_message,
                typename type
            >
            struct has_to_osc<
                osc_message,
                type,
                bbb::void_t<
                    decltype(
                        std::declval<type>()
                            .to_osc(std::declval<const osc_message &>())
                    )
                >
            >
            : std::true_type {};

#define BBB_DEF_CONV_OSC(TYPE); \
            template <typename osc_message> \
            inline std::size_t to_osc(osc_message &m, TYPE v) { \
                m.push(v); \
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

            template <
                typename osc_message,
                typename type
            >
            inline auto to_osc(const osc_message &m,
                               type &v)
                -> bbb::enable_if_t<
                    has_to_osc<osc_message, type>::value,
                    std::size_t
                >
            {
                return v.to_osc(m);
            }

            struct to_osc_fn {
                template <
                    typename osc_message,
                    typename value_type
                >
                auto operator()(osc_message &m,
                                value_type &&v) const
                    -> decltype(to_osc(m, std::forward<value_type>(v)))
                {
                    return to_osc(m, std::forward<value_type>(v));
                }
            }; // struct from_osc_fn
        }; // namespace detail

        namespace {
            constexpr const auto& to_osc = bbb::osc::convert::detail::static_const<convert::to_osc_fn>::value;
        };
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_message_convert_to_osc_hpp */
