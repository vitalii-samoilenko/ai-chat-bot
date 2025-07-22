#ifndef AI_CHAT_COMMANDS_REMOVE_IPP
#define AI_CHAT_COMMANDS_REMOVE_IPP

#include <ctime>

namespace ai {
namespace chat {
namespace commands {

template<typename History>
remove<History>::remove(::ai::chat::histories::observable<History> &history)
    : _history{ history }
    , _range_parser{
        R"((?<year>[0-9]{4})-(?<month>[0-9]{2})-(?<day>[0-9]{2}))"
        R"(T)"
        R"((?<hours>[0-9]{2}):(?<minutes>[0-9]{2}):(?<seconds>[0-9]{2}))"
        R"( )"
        R"((?<year>[0-9]{4})-(?<month>[0-9]{2})-(?<day>[0-9]{2}))"
        R"(T)"
        R"((?<hours>[0-9]{2}):(?<minutes>[0-9]{2}):(?<seconds>[0-9]{2}))"
    }
    , _timestamp_parser{ R"((?<timestamp>[0-9]+))" } {

};

template<typename History>
::std::string_view remove<History>::execute(::std::string_view args) {
    static time_t __epoch__{};
    static ::std::tm __local__epoch__{ *::std::localtime(&__epoch__) };
    ::std::tm f_tm{}, t_tm{};
    long long representation{};
    if (::RE2::FullMatch(args, _range_parser,
            &f_tm.tm_year, &f_tm.tm_mon, &f_tm.tm_mday,
            &f_tm.tm_hour, &f_tm.tm_min, &f_tm.tm_sec,
            &t_tm.tm_year, &t_tm.tm_mon, &t_tm.tm_mday,
            &t_tm.tm_hour, &t_tm.tm_min, &t_tm.tm_sec)) {
        f_tm.tm_year -= 1900; f_tm.tm_mon -= 1;
        t_tm.tm_year -= 1900; t_tm.tm_mon -= 1;
        f_tm.tm_hour += __local__epoch__.tm_hour; f_tm.tm_min += __local__epoch__.tm_min;
        t_tm.tm_hour += __local__epoch__.tm_hour; t_tm.tm_min += __local__epoch__.tm_min;
        ::std::chrono::nanoseconds from{ ::std::mktime(&f_tm) * 1000000000 };
        ::std::chrono::nanoseconds to{ ::std::mktime(&t_tm) * 1000000000 };
        if (to < from) {
            return ::std::string_view{};
        }
        ::ai::chat::histories::observable_iterator<History> first{ _history.lower_bound(from) };
        ::ai::chat::histories::observable_iterator<History> last{ _history.lower_bound(to) };
        _history.template erase<remove>(first, last);
        return remove::name;
    } else if (::RE2::FullMatch(args, _timestamp_parser,
            &representation)) {
        ::std::chrono::nanoseconds timestamp{ representation };
        ::ai::chat::histories::observable_iterator<History> pos{ _history.lower_bound(timestamp) };
        if (pos > timestamp) {
            return ::std::string_view{};
        }
        _history.template erase<remove>(pos);
        return remove::name;
    } else {
        return ::std::string_view{};
    }
};

} // commands
} // chat
} // ai

#endif
