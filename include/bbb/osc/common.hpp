//
//  bbb/osc/common.hpp
//  OSCSocketDev
//
//  Created by 2bit on 2022/08/22.
//

#ifndef bbb_osc_common_hpp
#define bbb_osc_common_hpp

#include <oscpp/detail/tag.h>
#include <oscpp/server.hpp>
#include <oscpp/client.hpp>
#include <oscpp/print.hpp>

namespace bbb {
    namespace osc {
        using OSCPP::Tag;
        using OSCPP::TagType;
        using OSCPP::TagName;
    };
};

namespace OSCPP {
    namespace Server {
        class Packet;
    };
};

#endif /* bbb_osc_common_hpp */
