#ifndef AI_CHAT_CLIENTS_DETAIL_CONNECTION_IPP
#define AI_CHAT_CLIENTS_DETAIL_CONNECTION_IPP

#include <iostream>

namespace ai {
namespace chat {
namespace clients {
namespace detail {

template<typename Handler>
connection<Handler>::connection(console<Handler> &handler)
    : _handler{ handler }
    , _username{}
    , _re_command_wargs{ R"(!(?<command>[a-z]+) (?<args>.+))" }
    , _re_command{ R"(!(?<command>[a-z]+))" }
    , _re_message{ R"(.+)" }
    INIT_LOGGER("ai_chat_clients_console")
    INIT_TRACER("ai_chat_clients_console") {

};

template<typename Handler>
void connection<Handler>::on_init() {

};
template<typename Handler>
void connection<Handler>::on_connect(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_connect", root, (*this))
    STOP_SPAN(span)
};
template<typename Handler>
void connection<Handler>::on_read() {
    START_SPAN(operation, "on_read", (*this))
    ::std::string line{};
    ::std::getline(::std::cin, line);
    LOG_INFO(line.data(), line.size(), operation, (*this));
    ::std::string_view arg1{};
    ::std::string_view arg2{};
    if (::RE2::FullMatch(line, _re_command_wargs,
            &arg1, &arg2)
        || ::RE2::FullMatch(line, _re_command,
            &arg1)) {
        START_SUBSPAN(span, "command", operation, (*this))
        START_SUBSCOPE(scope, "on_command", span, (*this))
        _handler.on_command(command{
            _username,
            arg1, arg2
        });
    } else if (::RE2::FullMatch(line, _re_message)) {
        START_SUBSPAN(span, "message", operation, (*this))
        START_SUBSCOPE(scope, "on_message", span, (*this))
        _handler.on_message(message{
            _username,
            line
        });
    }
    STOP_SPAN(operation)
};
template<typename Handler>
void connection<Handler>::on_disconnect(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_disconnect", root, (*this))
};
template<typename Handler>
void connection<Handler>::on_send(
    DECLARE_ONLY_SPAN(root)) {
    START_SUBSPAN(span, "on_send", root, (*this))
    ::std::cout << _message_content << ::std::endl;
};

} // detail
} // clients
} // chat
} // ai

#endif
