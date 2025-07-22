#ifndef AI_CHAT_COMMANDS_FIND_IPP
#define AI_CHAT_COMMANDS_FIND_IPP

#include <ctime>

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
    , _tag_parser{ R"( (?<name>[a-zA-Z0-9_.]+)=(?<value>[a-zA-Z0-9_.]*))" }
    , _buffer{} {

};

template<typename History, size_t Limit>
::std::string_view find<History, Limit>::execute(::std::string_view args) {
    static time_t __epoch__{};
    static ::std::tm __local__epoch__{ *::std::localtime(&__epoch__) };
    ::std::tm f_tm{}, t_tm{};
    ::std::string_view tags;
    if (!::RE2::FullMatch(args, _range_parser,
            &f_tm.tm_year, &f_tm.tm_mon, &f_tm.tm_mday,
            &f_tm.tm_hour, &f_tm.tm_min, &f_tm.tm_sec,
            &t_tm.tm_year, &t_tm.tm_mon, &t_tm.tm_mday,
            &t_tm.tm_hour, &t_tm.tm_min, &t_tm.tm_sec,
            &tags)) {
        return ::std::string_view{};
    }
    f_tm.tm_year -= 1900; f_tm.tm_mon -= 1;
    t_tm.tm_year -= 1900; t_tm.tm_mon -= 1;
    f_tm.tm_hour += __local__epoch__.tm_hour; f_tm.tm_min += __local__epoch__.tm_min;
    t_tm.tm_hour += __local__epoch__.tm_hour; t_tm.tm_min += __local__epoch__.tm_min;
    ::std::chrono::nanoseconds from{ ::std::mktime(&f_tm) * 1000000000 };
    ::std::chrono::nanoseconds to{ ::std::mktime(&t_tm) * 1000000000 };
    if (to < from) {
        return ::std::string_view{};
    }
    ::ai::chat::histories::observable_iterator<History> pos{ _history.lower_bound(from) };
    ::std::string_view tag_name{};
    ::std::string_view tag_value{};
    while (::RE2::Consume(&tags, _tag_parser,
            &tag_name, &tag_value)) {
        pos &= ::ai::chat::histories::tag{
            tag_name,
            tag_value
        };
    }
    _buffer.clear();
    size_t i{ 0 };
    for (; i < Limit && ::ai::chat::histories::operator<(pos, to); ++i, ++pos) {
        ::ai::chat::histories::message message{ *pos };
        _buffer += "-" + ::std::to_string(message.timestamp.count());
    }
    if (i == Limit || _buffer.empty()) {
        _buffer += "-";
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
