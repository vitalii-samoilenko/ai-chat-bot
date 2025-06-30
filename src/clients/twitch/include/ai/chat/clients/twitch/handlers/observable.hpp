#ifndef AI_CHAT_CLIENTS_TWITCH_HANDLERS_OBSERVABLE_HPP
#define AI_CHAT_CLIENTS_TWITCH_HANDLERS_OBSERVABLE_HPP

#include <typeinfo>
#include <unordered_map>

#include "ai/chat/clients/twitch/irc.hpp"

namespace ai {
namespace chat {
namespace clients {
namespace twitch {
namespace handlers {

class observable {
public:
    observable() = default;
    observable(const observable&) = delete;
    observable(observable&&) = delete;

    ~observable() = default;

    observable& operator=(const observable&) = delete;
    observable& operator=(observable&&) = delete;

    class slot {
        friend observable;

    public:
        slot() = delete;
        slot(const slot&) = delete;
        slot(slot&&);

        ~slot();

        slot& operator=(const slot&) = delete;
        slot& operator=(slot&&);

        template<typename Action>
        void on_message(Action&& callback);

    private:
        slot(const ::std::type_info* p_observer, observable* p_target);

        const ::std::type_info* _p_observer;
        observable* _p_target;
    };

    using slot_type = slot;

    template<typename Observer>
    slot_type subscribe();

private:
    class subscription;
    friend irc<observable>;

    ::std::unordered_map<const ::std::type_info*, subscription> _subscriptions;
};

} // handlers
} // twitch
} // clients
} // chat
} // ai

#include "impl/observable.ipp"

#endif
