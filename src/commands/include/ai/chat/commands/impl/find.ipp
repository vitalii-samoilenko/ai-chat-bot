#ifndef AI_CHAT_COMMANDS_FIND_IPP
#define AI_CHAT_COMMANDS_FIND_IPP

namespace ai {
namespace chat {
namespace commands {

template<typename History, size_t Limit>
find<History, Limit>::find(::ai::chat::histories::observable<History> &history)
    : _history{ history }
    , _range_parser{
        R"((?<year>[0-9]{4})-(?<month>[0-9]{2})-(?<day>[0-9]{2}))"
        R"(T)"
        R"((?<hours>[0-9]{2}):(?<minutes>[0-9]{2}):(?<seconds>[0-9]{2}))"
        R"( )"
        R"((?<year>[0-9]{4})-(?<month>[0-9]{2})-(?<day>[0-9]{2}))"
        R"(T)"
        R"((?<hours>[0-9]{2}):(?<minutes>[0-9]{2}):(?<seconds>[0-9]{2}))"
        R"((?<tags>.*))"
    }
    , _tag_parser{ R"( (?<name>[a-zA-Z0-9_.]+)=(?<value>)[a-zA-Z0-9_.]*)" }
    , _buffer{} {

};

template<typename History, size_t Limit>
::std::string_view find<History, Limit>::execute(::std::string_view args) {
    long long f_year{}, f_month{}, f_day{},
        f_hour{}, f_minute{}, f_second{},
        t_year{}, t_month{}, t_day{},
        t_hour{}, t_minute{}, t_second{};
    ::std::string_view tags;
    if (!::RE2::FullMatch(args, _range_parser,
            &f_year, &f_month, &f_day,
            &f_hour, &f_minute, &f_second,
            &t_year, &t_month, &t_day,
            &t_hour, &t_minute, &t_second,
            &tags)) {
        return ::std::string_view{};
    }
    ::std::chrono::nanoseconds from{
        ::std::chrono::years{ f_year - 1970 } + ::std::chrono::months{ f_month } + ::std::chrono::days{ f_day }
        + ::std::chrono::hours{ f_hour } + ::std::chrono::minutes{ f_minute } + ::std::chrono::seconds{ f_second }
    };
    ::std::chrono::nanoseconds to{
        ::std::chrono::years{ t_year - 1970 } + ::std::chrono::months{ t_month } + ::std::chrono::days{ t_day }
        + ::std::chrono::hours{ t_hour } + ::std::chrono::minutes{ t_minute } + ::std::chrono::seconds{ t_second }
    };
    if (to < from) {
        return ::std::string_view{};
    }
    ::ai::chat::histories::observable_iterator<History> pos{ _history.lower_bound(from) };
    ::std::string_view tag_name{};
    ::std::string_view tag_value{};
    while (::RE2::Consume(tags,
            &tag_name, &tag_value)) {
        pos &= ::ai::chat::histories::tag{
            tag_name,
            tag_value
        };
    }
    _buffer.clear();
    size_t i{ 0 };
    for (; i < Limit && pos < to; ++i) {
        ::ai::chat::histories::message message{ *pos };
        _buffer += "-" + ::to_string(message.timestamp.count());
    }
    if (i == Limit || _buffer.empty()) {
        _buffer += "-"
    }
    return ::std::string_view{
        _buffer.data() + 1,
        _buffer.size() - 1
    };
};

} // commands
} // chat
} // ai

#endif
