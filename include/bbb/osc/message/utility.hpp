/* **** **** **** **** **** **** **** **** *
 *
 *         _/        _/        _/
 *        _/_/_/    _/_/_/    _/_/_/
 *       _/    _/  _/    _/  _/    _/
 *      _/    _/  _/    _/  _/    _/
 *     _/_/_/    _/_/_/    _/_/_/
 *
 * bit by bit
 * bbb/osc/message/utility.hpp
 *
 * author: ISHII 2bit
 * mail:   bit_by_bit@2bit.jp
 *
 * **** **** **** **** **** **** **** **** */

#pragma once

#ifndef bbb_osc_message_utility_hpp
#define bbb_osc_message_utility_hpp

#include <bbb/osc/message/message.hpp>

#include <bbb/thread/threaded_queue.hpp>

#include <bbb/core.hpp>

#include <memory>
#include <functional>

namespace bbb {
    namespace osc {
        template <typename Fn>
        constexpr bool is_argument_message() {
            return is_argument_same_to<Fn, bbb::osc::message>();
        };

        class abstract_threaded_callback {
        public:
            virtual void process(bbb::osc::message &) = 0;
            virtual void call() = 0;
            using ref = std::shared_ptr<abstract_threaded_callback>;
        };

        template <typename Prepared>
        using thread_process = std::function<Prepared(bbb::osc::message &)>;
        template <typename Prepared>
        using prepared_callback = std::function<void(Prepared &&)>;

        template <typename Prepared>
        class threaded_callback : public abstract_threaded_callback {
            bbb::threaded_queue<Prepared> queued_messages;
            thread_process<Prepared> thread_proc;
            prepared_callback<Prepared> callback;
            
        public:
            threaded_callback(thread_process<Prepared> thread_proc,
                              prepared_callback<Prepared> callback)
            : thread_proc(thread_proc)
            , callback(callback) {}
            
            virtual void process(bbb::osc::message &mess) {
                Prepared v(thread_proc(mess));
                queued_messages.send(std::move(v));
            }
            virtual void call() {
                Prepared v;
                while(!queued_messages.empty()) {
                    queued_messages.receive(v);
                    callback(std::move(v));
                }
            }
        };

        template <typename Prepared>
        abstract_threaded_callback::ref make_threaded_callback(thread_process<Prepared> thread_process,
                                                               prepared_callback<Prepared> callback)
        {
            return abstract_threaded_callback::ref(new threaded_callback<Prepared>(thread_process, callback));
        }
    }; // namespace osc
}; // namespace bbb

#endif /* bbb_osc_message_utility_hpp */
