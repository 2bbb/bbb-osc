//
//  bbb/osc/message_utility.hpp
//
//  Created by 2bit on 2022/08/22.
//

#ifndef bbb_osc_message_utility_hpp
#define bbb_osc_message_utility_hpp

#include <bbb/osc/message.hpp>
#include <bbb/core.hpp>

#include <msd/channel.hpp>

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
            msd::channel<Prepared> channel;
            thread_process<Prepared> thread_proc;
            prepared_callback<Prepared> callback;
            
        public:
            threaded_callback(thread_process<Prepared> thread_proc,
                              prepared_callback<Prepared> callback)
            : thread_proc(thread_proc)
            , callback(callback) {}
            
            virtual void process(bbb::osc::message &mess) {
                Prepared v(thread_proc(mess));
                std::move(v) >> channel;
            }
            virtual void call() {
                Prepared v;
                while(!channel.empty()) {
                    operator<<(v, channel);
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
