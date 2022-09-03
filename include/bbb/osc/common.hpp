/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/common.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

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
