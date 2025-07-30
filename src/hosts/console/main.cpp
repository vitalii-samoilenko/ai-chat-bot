#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "boost/json.hpp"

#ifdef CLIENT_TWITCH
#include "ai/chat/clients/auth.hpp"
#include "ai/chat/clients/twitch.hpp"
#else
#include "ai/chat/clients/console.hpp"
#endif
#include "ai/chat/clients/observable.hpp"
#include "ai/chat/adapters/openai.hpp"
#include "ai/chat/histories/sqlite.hpp"
#include "ai/chat/histories/observable.hpp"
#include "ai/chat/moderators/sqlite.hpp"
#include "ai/chat/commands.hpp"
#ifdef CLIENT_TWITCH
#include "ai/chat/binders/twitch.hpp"
#else
#include "ai/chat/binders/console.hpp"
#endif
#include "ai/chat/binders/openai.hpp"
#include "ai/chat/telemetry.hpp"

#include "sqlite3.h"

::boost::json::monotonic_resource buffer{};
::boost::json::value config{};
void init_config(::std::string_view filename) {
    ::std::ifstream input{ filename.data() };
    if (!input.is_open()) {
        throw ::std::invalid_argument{ "configuration not found" };
    }
    ::boost::json::stream_parser parser{ &buffer };
    ::std::string line{};
    while (::std::getline(input, line)) {
        parser.write_some(line);
    }
    config = parser.release();
};

SYSTEM_CALLBACKS()

int main(int argc, char* argv[]) {
    ::std::set_terminate([]()->void {
        try {
            ::std::exception_ptr e{ ::std::current_exception() };
            if (e) {
                ::std::rethrow_exception(e);
            }
        }
        catch (::std::exception const &e) {
            ::std::cerr << "Exception: " << e.what() << ::std::endl;
        }
        catch (...) {
            ::std::cerr << "Exception: unknown" << ::std::endl;
        }
        ::std::exit(EXIT_FAILURE);
    });

    init_config(argc == 2
        ? argv[1]
        : "config.json");

    #ifdef ENABLE_TELEMETRY
    ::boost::json::string &endpoint{ config.at("telemetry").at("endpoint").as_string() };
    INIT_TELEMETRY(endpoint, "ai_chat_hosts_console")
    #endif

    auto on_exit = ::boost::scope::make_scope_exit([]()->void {
        ::sqlite3_shutdown();
    });
    ::sqlite3_initialize();

    bool history_exists{ false };
    {
        ::std::ifstream file{ config.at("history").at("filename").as_string().c_str() };
        history_exists = file.is_open();
    }
    ::ai::chat::histories::observable<::ai::chat::histories::sqlite> history{
        config.at("history").at("filename").as_string()
    };
    if (!history_exists) {
        for (::boost::json::value const &message : config.at("context").as_array()) {
            ::std::vector<::ai::chat::histories::tag> tags{};
            for (::boost::json::value const &tag : message.at("tags").as_array()) {
                tags.emplace_back(
                    tag.at("name").as_string(),
                    tag.at("value").as_string());
            }
            history.template insert<decltype(main)>(::ai::chat::histories::message{
                ::std::chrono::nanoseconds{},
                message.at("content").as_string(),
                tags
            });
        }
    }

    char const USERNAME[]{ "{username}" };
    char const CONTENT[]{ "{content}" };
    auto f_content = [
        pattern = ::std::string{ config.at("pattern").as_string() },
        pos_p_username = config.at("pattern").as_string().find(USERNAME), _pos_p_content = config.at("pattern").as_string().find(CONTENT),
        USERNAME_SIZE = ::std::size(USERNAME), CONTENT_SIZE = ::std::size(CONTENT),
        formatted_content = ::std::string{}
    ](::std::string_view username, ::std::string_view content) mutable ->::std::string_view {
        formatted_content = pattern;
        size_t pos_p_content{ _pos_p_content };
        if (!(pos_p_username == ::std::string::npos)) {
            formatted_content.replace(pos_p_username, USERNAME_SIZE - 1, username);
            if (!(_pos_p_content == ::std::string::npos) && pos_p_username < _pos_p_content) {
                pos_p_content += username.size() - (USERNAME_SIZE - 1);
            }
        }
        if (!(pos_p_content == ::std::string::npos)) {
            formatted_content.replace(pos_p_content, CONTENT_SIZE - 1, content);
        }
        return formatted_content;
    };
    auto f_apology = [
        apology = ::std::string{ config.at("apology").as_string() },
        pos_a_username = config.at("apology").as_string().find(USERNAME),
        USERNAME_SIZE = ::std::size(USERNAME),
        formatted_apology = ::std::string{}
    ](::std::string_view username) mutable ->::std::string_view {
        formatted_apology = apology;
        if (!(pos_a_username == ::std::string::npos)) {
            formatted_apology.replace(pos_a_username, USERNAME_SIZE - 1, username);
        }
        return formatted_apology;
    };

    ::ai::chat::adapters::openai adapter{
        config.at("adapter").at("address").as_string(),
        ::std::chrono::milliseconds{ config.at("adapter").at("timeout").as_int64() },
        ::std::chrono::milliseconds{ config.at("adapter").at("delay").as_int64() },
        static_cast<size_t>(config.at("adapter").at("limits").at("completion").as_int64()),
        static_cast<size_t>(config.at("adapter").at("limits").at("total").as_int64())
    };
    {
        ::ai::chat::histories::observable_iterator<::ai::chat::histories::sqlite> pos{ history.begin() };
        ::ai::chat::histories::observable_iterator<::ai::chat::histories::sqlite> end{ history.end() };
        adapter.reserve(static_cast<size_t>(end - pos));
        for (; !(pos == end); ++pos) {
            ::ai::chat::histories::message history_message{ *pos };
            ::ai::chat::histories::tag const *username_tag{ nullptr };
            for (::ai::chat::histories::tag const &tag : history_message.tags) {
                if (tag.name == "user.name") {
                    username_tag = &tag;
                    break;
                }
            }
            if (username_tag) {
                if (username_tag->value == config.at("botname").as_string()) {
                    adapter.push_back(::ai::chat::adapters::message{
                        ::ai::chat::adapters::role::assistant,
                        history_message.content
                    });
                } else {
                    adapter.push_back(::ai::chat::adapters::message{
                        ::ai::chat::adapters::role::user,
                        f_content(username_tag->value, history_message.content)
                    });
                }
            } else {
                adapter.push_back(::ai::chat::adapters::message{
                    ::ai::chat::adapters::role::system,
                    history_message.content
                });
            }
        }
    }

    #ifdef CLIENT_TWITCH
    ::ai::chat::clients::observable<::ai::chat::clients::twitch> client{
        config.at("client").at("address").as_string(),
        ::std::chrono::milliseconds{ config.at("client").at("timeout").as_int64() },
        ::std::chrono::milliseconds{ config.at("client").at("delay").as_int64() },
        0
    };
    #else
    ::ai::chat::clients::observable<::ai::chat::clients::console> client{};
    #endif

    bool moderator_exists{ false };
    {
        ::std::ifstream file{ config.at("moderator").at("filename").as_string().c_str() };
        moderator_exists = file.is_open();
    }
    ::ai::chat::moderators::sqlite moderator{
        config.at("moderator").at("filename").as_string()
    };
    if (!moderator_exists) {
        for (::boost::json::value const &username : config.at("administrators").as_array()) {
            moderator.admin(username.as_string());
        }
        for (::boost::json::value const &username : config.at("allowed").as_array()) {
            moderator.allow(username.as_string());
        }
        for (::boost::json::value const &filter : config.at("filters").as_array()) {
            moderator.filter(
                filter.at("name").as_string(),
                filter.at("pattern").as_string()
            );
        }
    }

    ::ai::chat::commands::executor<
    ::ai::chat::commands::allow<::ai::chat::moderators::sqlite>,
    ::ai::chat::commands::ban<::ai::chat::moderators::sqlite>,
    ::ai::chat::commands::content<::ai::chat::histories::sqlite>,
    ::ai::chat::commands::deny<::ai::chat::moderators::sqlite>,
    ::ai::chat::commands::edit<::ai::chat::histories::sqlite>,
    ::ai::chat::commands::find<::ai::chat::histories::sqlite, 25>,
    ::ai::chat::commands::instruct<::ai::chat::histories::sqlite>,
    #ifdef CLIENT_TWITCH
    ::ai::chat::commands::join<::ai::chat::clients::twitch>,
    ::ai::chat::commands::leave<::ai::chat::clients::twitch>,
    #endif
    ::ai::chat::commands::mod<::ai::chat::moderators::sqlite>,
    ::ai::chat::commands::remove<::ai::chat::histories::sqlite>,
    ::ai::chat::commands::timeout<::ai::chat::moderators::sqlite>,
    ::ai::chat::commands::unban<::ai::chat::moderators::sqlite>,
    ::ai::chat::commands::unmod<::ai::chat::moderators::sqlite>
    > executor{
        moderator,
        moderator,
        history,
        moderator,
        history,
        history,
        history,
    #if CLIENT_TWITCH
        client,
        client,
    #endif
        moderator,
        history,
        moderator,
        moderator,
        moderator
    };

    #ifdef CLIENT_TWITCH
    auto client_binding = ::ai::chat::binders::twitch<::ai::chat::histories::sqlite>::bind(history, client,
        moderator, executor,
        config.at("botname").as_string());
    #else
    auto client_binding = ::ai::chat::binders::console<::ai::chat::histories::sqlite>::bind(history, client,
        moderator, executor,
        config.at("botname").as_string());
    #endif
    auto adapter_binding = ::ai::chat::binders::openai<::ai::chat::histories::sqlite>::bind(history, adapter,
        moderator,
        config.at("adapter").at("model").as_string(), config.at("adapter").at("key").as_string(),
        static_cast<size_t>(config.at("adapter").at("skip").as_int64()), ::std::chrono::hours{ config.at("adapter").at("range").as_int64() },
        f_content, static_cast<size_t>(config.at("retries").as_int64()), f_apology,
        config.at("botname").as_string());

    #ifdef CLIENT_TWITCH
    ::std::string access_token{};
    {
        ::ai::chat::clients::auth auth{
            config.at("client").at("auth").at("address").as_string(),
            ::std::chrono::milliseconds{ config.at("client").at("auth").at("timeout").as_int64() },
        };
        ::ai::chat::clients::token_context access_context{
            auth.refresh_token(
                config.at("client").at("auth").at("client_id").as_string(),
                config.at("client").at("auth").at("client_secret").as_string(),
                config.at("client").at("auth").at("refresh_token").as_string())
        };
        access_token = access_context.access_token;
    }
    client.connect(
        config.at("botname").as_string(),
        access_token);
    #else
    client.connect(
        config.at("client").at("username").as_string());
    #endif
    config.emplace_null();
    buffer.release();
    ::std::cout << "Waiting for shutdown signal..." << ::std::endl;
    client.attach();

    return EXIT_SUCCESS;
};
