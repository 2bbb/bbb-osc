//
//  bbb/osc/message/convert/detail.hpp
//
//  Created by 2bit on 2022/08/31.
//

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
