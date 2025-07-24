#include <fstream>
#include <iostream>
#include <exception>
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

    ::ai::chat::adapters::openai adapter{
        config.at("adapter").at("address").as_string(),
        ::std::chrono::milliseconds{ config.at("adapter").at("timeout").as_int64() },
        ::std::chrono::milliseconds{ config.at("adapter").at("delay").as_int64() },
        static_cast<size_t>(config.at("adapter").at("limit").as_int64())
    };
    {
        ::ai::chat::histories::observable_iterator<::ai::chat::histories::sqlite> pos{ history.begin() };
        ::ai::chat::histories::observable_iterator<::ai::chat::histories::sqlite> end{ history.end() };
        adapter.reserve(static_cast<size_t>(end - pos));
        for (; !(pos == end); ++pos) {
            ::ai::chat::histories::message message{ *pos };
            ::ai::chat::histories::tag const *username_tag{ nullptr };
            for (::ai::chat::histories::tag const &tag : message.tags) {
                if (tag.name == "user.name") {
                    username_tag = &tag;
                    break;
                }
            }
            adapter.push_back(::ai::chat::adapters::message{
                username_tag
                    ? username_tag->value == config.at("botname").as_string()
                        ? ::ai::chat::adapters::role::assistant
                        : ::ai::chat::adapters::role::user
                    : ::ai::chat::adapters::role::system,
                message.content
            });
        }
    }

    #ifdef CLIENT_TWITCH
    ::ai::chat::clients::auth auth{
        config.at("client").at("auth").at("address").as_string(),
        ::std::chrono::milliseconds{ config.at("client").at("auth").at("timeout").as_int64() },
    };
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
        config.at("pattern").as_string(), static_cast<size_t>(config.at("retries").as_int64()), config.at("apology").as_string(),
        config.at("botname").as_string());

    #ifdef CLIENT_TWITCH
    ::ai::chat::clients::token_context access_context{
        auth.refresh_token(
            config.at("client").at("auth").at("client_id").as_string(),
            config.at("client").at("auth").at("client_secret").as_string(),
            config.at("client").at("auth").at("refresh_token").as_string())
    };
    client.connect(
        config.at("botname").as_string(),
        access_context.access_token);
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
