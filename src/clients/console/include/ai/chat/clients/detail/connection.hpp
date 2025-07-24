#ifndef AI_CHAT_CLIENTS_DETAIL_CONNECTION_HPP
#define AI_CHAT_CLIENTS_DETAIL_CONNECTION_HPP

#include <string>

#include "re2/re2.h"

#include "ai/chat/telemetry.hpp"

namespace ai {
namespace chat {
namespace clients {
namespace detail {

template<typename Handler>
class connection {
private:
    friend console<Handler>;

    connection() = delete;
    connection(connection const &other) = delete;
    connection(connection &&other) = delete;

    ~connection() = default;

    connection & operator=(connection const &other) = delete;
    connection & operator=(connection &&other) = delete;

    explicit connection(console<Handler> &handler);

    void on_init();
    void on_connect(
        DECLARE_ONLY_SPAN(root));
    void on_read();
    void on_disconnect(
        DECLARE_ONLY_SPAN(root));
    void on_send(
        DECLARE_ONLY_SPAN(root));

    console<Handler>& _handler;
    ::std::string _username;
    ::std::string _message_content;
    ::RE2 _re_command_wargs;
    ::RE2 _re_command;
    ::RE2 _re_message;

    DECLARE_LOGGER()
    DELCARE_TRACER()
};

} // detail
} // clients
} // chat
} // ai

#include "impl/connection.ipp"

#endif
