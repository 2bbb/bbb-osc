/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_hpp
#define bbb_osc_hpp

#include <bbb/osc/custom_sender.hpp>
#include <bbb/osc/custom_receiver.hpp>

#if !BBB_OSC_DISABLE_ASIO
#   include <bbb/osc/sender.hpp>
#   include <bbb/osc/receiver.hpp>
#endif

#include <bbb/osc/message.hpp>
#include <bbb/osc/argument.hpp>

#endif /* bbb_osc_hpp */
