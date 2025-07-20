#ifndef AI_CHAT_COMMANDS_REMOVE_IPP
#define AI_CHAT_COMMANDS_REMOVE_IPP

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
    long long f_year{}, f_month{}, f_day{},
        f_hour{}, f_minute{}, f_second{},
        t_year{}, t_month{}, t_day{},
        t_hour{}, t_minute{}, t_second{};
    long long representation{};
    if (::RE2::FullMatch(args, _range_parser,
            &f_year, &f_month, &f_day,
            &f_hour, &f_minute, &f_second,
            &t_year, &t_month, &t_day,
            &t_hour, &t_minute, &t_second)) {
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
        ::ai::chat::histories::observable_iterator<History> first{ _history.lower_bound(from) };
        ::ai::chat::histories::observable_iterator<History> last{ _history.lower_bound(to) };
        _history.template erase<remove>(first, last);
        return remove::name;
    } else if (::RE2::FullMatch(args, _timestamp_parser,
            &representation)) {
        ::std::chrono::nanoseconds timestamp{ representation };
        ::ai::chat::histories::observable_iterator<History> current{ _history.lower_bound(timestamp) };
        if (timestamp < pos) {
            return ::std::string_view{};
        }
        _history.template erase<remove>(current);
        return remove::name;
    } else {
        return ::std::string_view{};
    }
};

} // commands
} // chat
} // ai

#endif
