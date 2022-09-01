//
//  bbb/osc/message/convert/to_osc.hpp
//
//  Created by 2bit on 2022/08/31.
//

#ifndef bbb_osc_message_convert_to_osc_hpp
#define bbb_osc_message_convert_to_osc_hpp

#include <bbb/osc/message/convert/detail.hpp>

#include <bbb/osc/argument.hpp>

#include <bbb/osc/common.hpp>

namespace bbb {
    namespace osc {
        namespace convert {
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
