﻿using AI.Chat.Adapters.Extensions.GoogleAI;
using AI.Chat.Adapters.Extensions.OpenAI;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Http;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Polly;
using Polly.Extensions.Http;

namespace Microsoft.Extensions.DependencyInjection
{
    public static class ServiceCollection
    {
        public static IServiceCollection AddAIChat(this IServiceCollection services, IConfigurationManager configuration)
        {
            configuration.AddJsonFile(AI.Chat.Host.Defaults.JsonCommon, true);
            configuration.AddJsonFile(AI.Chat.Host.Defaults.JsonModerated, true);

            var commandOverrides = new System.Collections.Generic.Dictionary<System.Type, string>();
            var diagnostics = configuration.GetValue<bool>("Chat:Diagnostics");

            services.Configure<AI.Chat.Options.Bot>(
                configuration.GetSection("Chat:Bot"));
            services.Configure<AI.Chat.Options.Moderator>(
                configuration.GetSection("Chat:Moderator"));

            var scopeType = typeof(AI.Chat.Scopes.Slim);
            services.AddTransient(scopeType);
            if (diagnostics)
            {
                scopeType = typeof(AI.Chat.Scopes.Diagnostics.Trace<>)
                    .MakeGenericType(scopeType);
                services.AddTransient(scopeType);
                scopeType = typeof(AI.Chat.Scopes.Diagnostics.OpenClose<>)
                    .MakeGenericType(scopeType);
                services.AddTransient(scopeType);
            }

            services.AddSingleton<System.Collections.Generic.TimeSeries<AI.Chat.Record>>(
                serviceProvider => AI.Chat.Host.Helpers.LoadLog());

            var historyType = typeof(AI.Chat.Histories.Slim);
            services.AddSingleton(historyType);
            historyType = typeof(AI.Chat.Histories.Host.Persistent<>)
                .MakeGenericType(historyType);
            services.AddSingleton(historyType);

            System.Type adapterType = null;
            System.Type serviceType = null;
            switch (configuration.GetValue<string>("Chat:Adapter:Type"))
            {
                case nameof(AI.Chat.Adapters.OpenAI):
                    {
                        services.Configure<AI.Chat.Options.OpenAI.Adapter>(
                            configuration.GetSection("Chat:Adapter"));
                        services.AddSingleton<IOptions<AI.Chat.Options.Adapter>>(
                            serviceProvider => serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.OpenAI.Adapter>>());

                        services.AddSingleton<System.Collections.Generic.TimeSeries<OpenAI.Chat.ChatMessage>>(
                            serviceProvider =>
                            {
                                var records = serviceProvider
                                    .GetRequiredService<System.Collections.Generic.TimeSeries<AI.Chat.Record>>();
                                var messages = new System.Collections.Generic.TimeSeries<OpenAI.Chat.ChatMessage>(
                                    records.Start);
                                foreach (var entry in (System.Collections.Generic.IEnumerable<System.Collections.Generic.Entry<AI.Chat.Record>>)records)
                                {
                                    messages.Add(entry.Key, entry.Value.ToChatMessage());
                                }

                                return messages;
                            });

                        historyType = typeof(AI.Chat.Histories.OpenAI.Tracked<>)
                            .MakeGenericType(historyType);
                        services.AddSingleton(historyType);

                        services.AddTransient<OpenAI.Chat.ChatClient>(
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.OpenAI.Adapter>>()
                                    .Value;

                                return new OpenAI.Chat.ChatClient(
                                    options.Model.Name,
                                    new System.ClientModel.ApiKeyCredential(
                                        options.Client.ApiKey),
                                    options.Client
                                );
                            });

                        adapterType = typeof(AI.Chat.Adapters.OpenAI);
                        services.AddTransient(adapterType,
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.OpenAI.Adapter>>()
                                    .Value;
                                var client = serviceProvider
                                    .GetRequiredService<OpenAI.Chat.ChatClient>();
                                var messages = serviceProvider
                                    .GetRequiredService<System.Collections.Generic.TimeSeries<OpenAI.Chat.ChatMessage>>();

                                return new AI.Chat.Adapters.OpenAI(
                                    options,
                                    client,
                                    messages);
                            });
                    }
                    break;
                case nameof(AI.Chat.Adapters.GoogleAI):
                    {
                        services.Configure<AI.Chat.Options.GoogleAI.Adapter>(
                            configuration.GetSection("Chat:Adapter"));
                        services.AddSingleton<IOptions<AI.Chat.Options.Adapter>>(
                            serviceProvider => serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.GoogleAI.Adapter>>());

                        services.AddSingleton<System.Collections.Generic.TimeSeries<GoogleAI.Models.Content>>(
                            serviceProvider =>
                            {
                                var records = serviceProvider
                                    .GetRequiredService<System.Collections.Generic.TimeSeries<AI.Chat.Record>>();
                                var contents = new System.Collections.Generic.TimeSeries<GoogleAI.Models.Content>(
                                    records.Start);
                                foreach (var entry in (System.Collections.Generic.IEnumerable<System.Collections.Generic.Entry<AI.Chat.Record>>)records)
                                {
                                    contents.Add(entry.Key, entry.Value.ToContent());
                                }

                                return contents;
                            });

                        historyType = typeof(AI.Chat.Histories.GoogleAI.Tracked<>)
                            .MakeGenericType(historyType);
                        services.AddSingleton(historyType);
                        var cachedHistory = typeof(AI.Chat.Histories.GoogleAI.Cached<>)
                            .MakeGenericType(historyType);
                        var cachedHistoryType = historyType;
                        services.AddSingleton(cachedHistory,
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.GoogleAI.Adapter>>()
                                    .Value;
                                var history = serviceProvider
                                    .GetRequiredService(cachedHistoryType);

                                return System.Activator.CreateInstance(cachedHistory,
                                    options,
                                    history);
                            });
                        historyType = cachedHistory;

                        var googleClient = typeof(GoogleAI.Client);
                        services.AddHttpClient<GoogleAI.Client>(
                            (serviceProvider, httpClient) =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.GoogleAI.Adapter>>()
                                    .Value;

                                httpClient.BaseAddress = options.Client.BaseAddress;
                            })
                            .AddHttpMessageHandler(
                                serviceProvider =>
                                {
                                    var options = serviceProvider
                                        .GetRequiredService<IOptions<AI.Chat.Options.GoogleAI.Adapter>>()
                                        .Value;

                                    return new System.Net.Http.GoogleAI.ApiKeyHandler(
                                        options.Client.ApiKey);
                                })
                            .AddHttpMessageHandler(
                                serviceProvider =>
                                {
                                    var options = serviceProvider
                                        .GetRequiredService<IOptions<AI.Chat.Options.GoogleAI.Adapter>>()
                                        .Value;
                                    var asyncPolicy = HttpPolicyExtensions.HandleTransientHttpError()
                                        .WaitAndRetryAsync(options.Client.Retries);

                                    return new PolicyHttpMessageHandler(
                                        asyncPolicy);
                                });
                        if (diagnostics)
                        {
                            googleClient = typeof(GoogleAI.Diagnostics.Trace<>)
                                .MakeGenericType(googleClient);
                            services.AddTransient(googleClient);
                        }
                        services.AddTransient(typeof(GoogleAI.IClient),
                            serviceProvider => serviceProvider
                                .GetRequiredService(googleClient));

                        adapterType = typeof(AI.Chat.Adapters.GoogleAI);
                        services.AddTransient(adapterType,
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.GoogleAI.Adapter>>()
                                    .Value;
                                var client = serviceProvider
                                    .GetRequiredService<GoogleAI.IClient>();
                                var contents = serviceProvider
                                    .GetRequiredService<System.Collections.Generic.TimeSeries<GoogleAI.Models.Content>>();

                                return new AI.Chat.Adapters.GoogleAI(
                                    options,
                                    client,
                                    contents);
                            });
                    }
                    break;
                default:
                    throw new System.Exception("Adapter is not supported");
            }
            switch (configuration.GetValue<string>("Chat:Client:Type"))
            {
                case nameof(AI.Chat.Clients.Twitch):
                    {
                        configuration.AddJsonFile(AI.Chat.Host.Defaults.JsonOAuth, true);

                        services.Configure<AI.Chat.Options.Twitch.Client>(
                            configuration.GetSection("Chat:Client"));
                        services.AddSingleton<IOptions<AI.Chat.Options.Client>>(
                            serviceProvider => serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>());

                        adapterType = typeof(AI.Chat.Adapters.Twitch.Formatted<>)
                            .MakeGenericType(adapterType);
                        services.AddTransient(adapterType);

                        var authClientType = typeof(TwitchLib.Client.AuthClient);
                        services.AddHttpClient<TwitchLib.Client.AuthClient>(
                            (serviceProvider, httpClient) =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                    .Value;

                                httpClient.BaseAddress = options.Auth.BaseAddress;
                            });
                        services.AddTransient<TwitchLib.Communication.Interfaces.IClient, TwitchLib.Communication.Clients.WebSocketClient>(
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                    .Value;

                                return new TwitchLib.Communication.Clients.WebSocketClient(
                                    options.Communication);
                            });
                        var twitchClientType = typeof(TwitchLib.Client.TwitchClient);
                        services.AddTransient(twitchClientType);
                        if (diagnostics)
                        {
                            authClientType = typeof(TwitchLib.Client.Diagnostics.AuthClient<>)
                                .MakeGenericType(authClientType);
                            services.AddTransient(authClientType);

                            twitchClientType = typeof(TwitchLib.Client.Diagnostics.TwitchClient<>)
                                .MakeGenericType(twitchClientType);
                            services.AddTransient(twitchClientType);
                        }
                        twitchClientType = typeof(TwitchLib.Client.Diagnostics.ExceptionHandler<>)
                            .MakeGenericType(twitchClientType);
                        services.AddTransient(twitchClientType);
                        services.AddTransient(typeof(TwitchLib.Client.Interfaces.IAuthClient),
                            serviceProvider => serviceProvider
                                .GetRequiredService(authClientType));
                        services.AddKeyedSingleton(typeof(TwitchLib.Client.Interfaces.ITwitchClient), "user",
                            (serviceProvider, _) => serviceProvider
                                .GetRequiredService(twitchClientType));
                        services.AddKeyedSingleton(typeof(TwitchLib.Client.Interfaces.ITwitchClient), "moderator",
                            (serviceProvider, _) => serviceProvider
                                .GetRequiredService(twitchClientType));

                        services.AddKeyedSingleton(typeof(AI.Chat.IScope), "client",
                            (serviceProvider, _) => serviceProvider
                                .GetRequiredService(scopeType));

                        services.AddSingleton<AI.Chat.Clients.Twitch>(
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                    .Value;

                                var authClient = serviceProvider
                                    .GetRequiredService<TwitchLib.Client.Interfaces.IAuthClient>();
                                var userClient = serviceProvider
                                    .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("user");
                                var moderatorClient = serviceProvider
                                    .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("moderator");

                                var commandExecutor = serviceProvider
                                    .GetRequiredService<AI.Chat.ICommandExecutor>();
                                var client = serviceProvider
                                    .GetRequiredService<AI.Chat.IClient>();
                                var history = serviceProvider
                                    .GetRequiredService<AI.Chat.IHistory>();
                                var scope = serviceProvider
                                    .GetRequiredKeyedService<AI.Chat.IScope>("client");

                                return new AI.Chat.Clients.Twitch(
                                    options,

                                    authClient,
                                    userClient,
                                    moderatorClient,

                                    commandExecutor,
                                    client,
                                    history,
                                    scope);
                            });

                        var cheerful = typeof(AI.Chat.Commands.Twitch.Cheerful);
                        services.AddTransient(cheerful,
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                    .Value;

                                return new AI.Chat.Commands.Twitch.Cheerful(
                                    options);
                            });
                        if (diagnostics)
                        {
                            cheerful = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                                .MakeGenericType(cheerful);
                            services.AddTransient(cheerful);
                        }
                        cheerful = typeof(AI.Chat.Commands.ThreadSafeWrite<>)
                            .MakeGenericType(cheerful);
                        services.AddTransient(typeof(AI.Chat.ICommand),
                            serviceProvider =>
                            {
                                var command = serviceProvider
                                    .GetRequiredService(cheerful.GenericTypeArguments[0]);
                                var scope = serviceProvider
                                    .GetRequiredKeyedService<AI.Chat.IScope>("client");

                                return System.Activator.CreateInstance(cheerful,
                                    command,
                                    scope);
                            });
                        commandOverrides.Add(cheerful, nameof(AI.Chat.Commands.Twitch.Cheerful));

                        var delay = typeof(AI.Chat.Commands.Twitch.Delay);
                        services.AddTransient(delay,
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                    .Value;

                                return new AI.Chat.Commands.Twitch.Delay(
                                    options);
                            });
                        if (diagnostics)
                        {
                            delay = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                                .MakeGenericType(delay);
                            services.AddTransient(delay);
                        }
                        delay = typeof(AI.Chat.Commands.ThreadSafeWrite<>)
                            .MakeGenericType(delay);
                        services.AddTransient(typeof(AI.Chat.ICommand),
                            serviceProvider =>
                            {
                                var command = serviceProvider
                                    .GetRequiredService(delay.GenericTypeArguments[0]);
                                var scope = serviceProvider
                                    .GetRequiredKeyedService<AI.Chat.IScope>("client");

                                return System.Activator.CreateInstance(delay,
                                    command,
                                    scope);
                            });
                        commandOverrides.Add(delay, nameof(AI.Chat.Commands.Twitch.Delay));

                        var join = typeof(AI.Chat.Commands.Twitch.Join);
                        services.AddTransient(join,
                            serviceProvider =>
                            {
                                var client = serviceProvider
                                    .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("user");

                                return new AI.Chat.Commands.Twitch.Join(
                                    client);
                            });
                        if (diagnostics)
                        {
                            join = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                                .MakeGenericType(join);
                            services.AddTransient(join);
                        }
                        join = typeof(AI.Chat.Commands.ThreadSafeWrite<>)
                            .MakeGenericType(join);
                        services.AddTransient(join,
                            serviceProvider =>
                            {
                                var command = serviceProvider
                                    .GetRequiredService(join.GenericTypeArguments[0]);
                                var scope = serviceProvider
                                    .GetRequiredKeyedService<AI.Chat.IScope>("client");

                                return System.Activator.CreateInstance(join,
                                    command,
                                    scope);
                            });
                        services.AddTransient(typeof(AI.Chat.ICommand),
                            serviceProvider => serviceProvider
                                .GetRequiredService(join));
                        commandOverrides.Add(join, nameof(AI.Chat.Commands.Twitch.Join));

                        var leave = typeof(AI.Chat.Commands.Twitch.Leave);
                        services.AddTransient(leave,
                            serviceProvider =>
                            {
                                var client = serviceProvider
                                    .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("user");

                                return new AI.Chat.Commands.Twitch.Leave(
                                    client);
                            });
                        if (diagnostics)
                        {
                            leave = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                                .MakeGenericType(leave);
                            services.AddTransient(leave);
                        }
                        leave = typeof(AI.Chat.Commands.ThreadSafeWrite<>)
                            .MakeGenericType(leave);
                        services.AddTransient(leave,
                            serviceProvider =>
                            {
                                var command = serviceProvider
                                    .GetRequiredService(leave.GenericTypeArguments[0]);
                                var scope = serviceProvider
                                    .GetRequiredKeyedService<AI.Chat.IScope>("client");

                                return System.Activator.CreateInstance(leave,
                                    command,
                                    scope);
                            });
                        services.AddTransient(typeof(AI.Chat.ICommand),
                            serviceProvider => serviceProvider
                                .GetRequiredService(leave));
                        commandOverrides.Add(leave, nameof(AI.Chat.Commands.Twitch.Leave));

                        serviceType = typeof(AI.Chat.Host.Services.Twitch);
                        services.AddSingleton(serviceType,
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                    .Value;
                                var client = serviceProvider
                                    .GetRequiredService<AI.Chat.Clients.Twitch>();

                                return new AI.Chat.Host.Services.Twitch(
                                    options,
                                    client);
                            });
                    }
                    break;
                case nameof(AI.Chat.Clients.Console):
                    {
                        services.Configure<AI.Chat.Options.Console.Client>(
                            configuration.GetSection("Chat:Client"));
                        services.AddSingleton<IOptions<AI.Chat.Options.Client>>(
                            serviceProvider => serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.Console.Client>>());

                        services.AddSingleton<AI.Chat.Clients.Console>(
                            serviceProvider =>
                            {
                                var options = serviceProvider
                                    .GetRequiredService<IOptions<AI.Chat.Options.Console.Client>>()
                                    .Value;

                                var commandExecutor = serviceProvider
                                    .GetRequiredService<AI.Chat.ICommandExecutor>();
                                var client = serviceProvider
                                    .GetRequiredService<AI.Chat.IClient>();
                                var history = serviceProvider
                                    .GetRequiredService<AI.Chat.IHistory>();

                                return new AI.Chat.Clients.Console(
                                    options,

                                    commandExecutor,
                                    client,
                                    history);
                            });

                        serviceType = typeof(AI.Chat.Host.Services.Console);
                        services.AddSingleton(serviceType);
                    }
                    break;
                default:
                    throw new System.Exception("Client is not supported");
            }

            if (diagnostics)
            {
                serviceType = typeof(AI.Chat.Host.Services.Diagnostics.Trace<>)
                    .MakeGenericType(serviceType);
                services.AddSingleton(serviceType);
            }
            services.AddSingleton<IHostedService>(
                serviceProvider => (IHostedService)serviceProvider
                    .GetRequiredService(serviceType));

            if (diagnostics)
            {
                historyType = typeof(AI.Chat.Histories.Diagnostics.Trace<>)
                    .MakeGenericType(historyType);
                services.AddSingleton(historyType);
            }

            var limitedAdapter = typeof(AI.Chat.Adapters.Limited<>)
                .MakeGenericType(adapterType);
            var limitedHistory = historyType;
            services.AddTransient(limitedAdapter,
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Adapter>>()
                        .Value;
                    var adapter = serviceProvider
                        .GetRequiredService(limitedAdapter.GenericTypeArguments[0]);
                    var history = serviceProvider
                        .GetRequiredService(limitedHistory);

                    return System.Activator.CreateInstance(limitedAdapter,
                        options,
                        adapter,
                        history);
                });
            adapterType = limitedAdapter;
            if (diagnostics)
            {
                adapterType = typeof(AI.Chat.Adapters.Diagnostics.Trace<>)
                    .MakeGenericType(adapterType);
                services.AddTransient(adapterType);
            }
            var delayedAdapter = typeof(AI.Chat.Adapters.Delayed<>)
                .MakeGenericType(adapterType);
            services.AddTransient(delayedAdapter,
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Adapter>>()
                        .Value;
                    var adapter = serviceProvider
                        .GetRequiredService(delayedAdapter.GenericTypeArguments[0]);

                    return System.Activator.CreateInstance(delayedAdapter,
                        options,
                        adapter);
                });
            adapterType = delayedAdapter;
            if (diagnostics)
            {
                adapterType = typeof(AI.Chat.Adapters.Diagnostics.Trace<>)
                    .MakeGenericType(adapterType);
                services.AddTransient(adapterType);
                adapterType = typeof(AI.Chat.Adapters.Diagnostics.Tokens<>)
                    .MakeGenericType(adapterType);
                services.AddTransient(adapterType);
                adapterType = typeof(AI.Chat.Adapters.Diagnostics.ReplyLength<>)
                    .MakeGenericType(adapterType);
                services.AddTransient(adapterType);
            }
            adapterType = typeof(AI.Chat.Adapters.Diagnostics.Log<>)
                .MakeGenericType(adapterType);
            services.AddTransient(adapterType);
            services.AddTransient(typeof(AI.Chat.IAdapter),
                serviceProvider => serviceProvider
                    .GetRequiredService(adapterType));

            services.AddTransient<AI.Chat.IFilter>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Bot>>()
                        .Value;
                    var filters = new System.Collections.Generic.List<AI.Chat.IFilter>();
                    AI.Chat.IFilter filter = null;
                    foreach (var filterOptions in options.Filters)
                    {
                        switch (filterOptions.Type)
                        {
                            case AI.Chat.Options.FilterType.Length:
                                {
                                    filter = new AI.Chat.Filters.Length(
                                        filterOptions.Reason,
                                        int.Parse(filterOptions.Args[0]));
                                }
                                break;
                            case AI.Chat.Options.FilterType.Regex:
                                {
                                    filter = new AI.Chat.Filters.Regex(
                                        filterOptions.Reason,
                                        filterOptions.Args[0]);
                                }
                                break;
                            default:
                                throw new System.Exception("Filter is not supported");
                        }
                        if (diagnostics)
                        {
                            filter = (AI.Chat.IFilter)System.Activator.CreateInstance(
                                typeof(AI.Chat.Filters.Diagnostics.Trace<>)
                                    .MakeGenericType(filter.GetType()),
                                filter);
                        }
                        filters.Add(filter);
                    }

                    filter = new AI.Chat.Filters.Composite(filters);
                    if (diagnostics)
                    {
                        filter = (AI.Chat.IFilter)System.Activator.CreateInstance(
                            typeof(AI.Chat.Filters.Diagnostics.Trace<>)
                                .MakeGenericType(filter.GetType()),
                            filter);
                    }
                    var logFilter = typeof(AI.Chat.Filters.Diagnostics.Log<>)
                        .MakeGenericType(filter.GetType());
                    var logger = serviceProvider
                        .GetRequiredService(typeof(ILogger<>)
                            .MakeGenericType(logFilter));
                    return (AI.Chat.IFilter)System.Activator.CreateInstance(logFilter,
                        filter,
                        logger);
                });

            services.AddKeyedSingleton(typeof(AI.Chat.IScope), "bot",
                (serviceProvider, _) => serviceProvider
                    .GetRequiredService(scopeType));
            services.AddKeyedSingleton(typeof(AI.Chat.IScope), "moderator",
                (serviceProvider, _) => serviceProvider
                    .GetRequiredService(scopeType));

            var botType = typeof(AI.Chat.Bots.Filtered);
            var filteredHistory = historyType;
            services.AddSingleton(botType,
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Bot>>()
                        .Value;
                    var adapter = serviceProvider
                        .GetRequiredService<AI.Chat.IAdapter>();
                    var filter = serviceProvider
                        .GetRequiredService<AI.Chat.IFilter>();
                    var history = (AI.Chat.IHistory)serviceProvider
                        .GetRequiredService(filteredHistory);

                    return new AI.Chat.Bots.Filtered(
                        options,
                        adapter,
                        filter,
                        history);
                });
            if (diagnostics)
            {
                botType = typeof(AI.Chat.Bots.Diagnostics.Trace<>)
                    .MakeGenericType(botType);
                services.AddSingleton(botType);
                botType = typeof(AI.Chat.Bots.Diagnostics.MessageLength<>)
                    .MakeGenericType(botType);
                services.AddSingleton(botType);
            }
            var threadSafeBot = typeof(AI.Chat.Bots.ThreadSafe<>)
                .MakeGenericType(botType);
            services.AddSingleton(threadSafeBot,
                serviceProvider =>
                {
                    var bot = serviceProvider
                        .GetRequiredService(threadSafeBot.GenericTypeArguments[0]);
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("bot");

                    return System.Activator.CreateInstance(threadSafeBot,
                        bot,
                        scope);
                });
            botType = threadSafeBot;
            botType = typeof(AI.Chat.Bots.Diagnostics.Log<>)
                .MakeGenericType(botType);
            services.AddSingleton(botType);
            services.AddSingleton(typeof(AI.Chat.IBot),
                serviceProvider => serviceProvider
                    .GetRequiredService(botType));

            var moderatorType = typeof(AI.Chat.Moderators.Slim);
            services.AddSingleton(moderatorType,
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Moderator>>()
                        .Value;

                    return new AI.Chat.Moderators.Slim(
                        options);
                });
            var persistentModerator = typeof(AI.Chat.Moderators.Host.Persistent<>)
                .MakeGenericType(moderatorType);
            services.AddSingleton(persistentModerator,
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Moderator>>()
                        .Value;
                    var moderator = serviceProvider
                        .GetRequiredService(persistentModerator.GenericTypeArguments[0]);

                    return System.Activator.CreateInstance(persistentModerator,
                        options,
                        moderator);
                });
            moderatorType = persistentModerator;
            if (diagnostics)
            {
                moderatorType = typeof(AI.Chat.Moderators.Diagnostics.Trace<>)
                    .MakeGenericType(moderatorType);
                services.AddSingleton(moderatorType);
            }
            moderatorType = typeof(AI.Chat.Moderators.ThreadSafe<>)
                .MakeGenericType(moderatorType);
            services.AddSingleton(typeof(AI.Chat.IModerator),
                serviceProvider =>
                {
                    var moderator = serviceProvider
                        .GetRequiredService(moderatorType.GenericTypeArguments[0]);
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("moderator");

                    return System.Activator.CreateInstance(moderatorType,
                        moderator,
                        scope);
                });

            var clientType = typeof(AI.Chat.Clients.Slim);
            services.AddSingleton(clientType,
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Client>>()
                        .Value;
                    var moderator = serviceProvider
                        .GetRequiredService<AI.Chat.IModerator>();
                    var bot = serviceProvider
                        .GetRequiredService<AI.Chat.IBot>();
                    var history = serviceProvider
                        .GetRequiredService<AI.Chat.IHistory>();

                    return new AI.Chat.Clients.Slim(
                        options,
                        moderator,
                        bot,
                        history);
                });
            if (diagnostics)
            {
                clientType = typeof(AI.Chat.Clients.Diagnostics.Trace<>)
                    .MakeGenericType(clientType);
                services.AddSingleton(clientType);
            }
            services.AddSingleton(typeof(AI.Chat.IClient),
                serviceProvider => serviceProvider
                    .GetRequiredService(clientType));

            var allow = typeof(AI.Chat.Commands.Allow);
            services.AddTransient(allow);
            if (diagnostics)
            {
                allow = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(allow);
                services.AddTransient(allow);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(allow));
            commandOverrides.Add(allow, nameof(AI.Chat.Commands.Allow));

            var ban = typeof(AI.Chat.Commands.Ban);
            services.AddTransient(ban);
            if (diagnostics)
            {
                ban = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(ban);
                services.AddTransient(ban);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(ban));
            commandOverrides.Add(ban, nameof(AI.Chat.Commands.Ban));

            var demote = typeof(AI.Chat.Commands.Demote);
            services.AddTransient(demote);
            if (diagnostics)
            {
                demote = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(demote);
                services.AddTransient(demote);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(demote));
            commandOverrides.Add(demote, nameof(AI.Chat.Commands.Demote));

            var edit = typeof(AI.Chat.Commands.Edit);
            services.AddTransient(edit);
            if (diagnostics)
            {
                edit = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(edit);
                services.AddTransient(edit);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(edit));
            commandOverrides.Add(edit, nameof(AI.Chat.Commands.Edit));

            var find = typeof(AI.Chat.Commands.Find);
            services.AddTransient(find);
            if (diagnostics)
            {
                find = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(find);
                services.AddTransient(find);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(find));
            commandOverrides.Add(find, nameof(AI.Chat.Commands.Find));

            var get = typeof(AI.Chat.Commands.Get);
            services.AddTransient(get);
            if (diagnostics)
            {
                get = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(get);
                services.AddTransient(get);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(get));
            commandOverrides.Add(get, nameof(AI.Chat.Commands.Get));

            var instruct = typeof(AI.Chat.Commands.Instruct);
            services.AddTransient(instruct);
            if (diagnostics)
            {
                instruct = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(instruct);
                services.AddTransient(instruct);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(instruct));
            commandOverrides.Add(instruct, nameof(AI.Chat.Commands.Instruct));

            var mod = typeof(AI.Chat.Commands.Mod);
            services.AddTransient(mod);
            if (diagnostics)
            {
                mod = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(mod);
                services.AddTransient(mod);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(mod));
            commandOverrides.Add(mod, nameof(AI.Chat.Commands.Mod));

            var promote = typeof(AI.Chat.Commands.Promote);
            services.AddTransient(promote);
            if (diagnostics)
            {
                promote = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(promote);
                services.AddTransient(promote);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(promote));
            commandOverrides.Add(promote, nameof(AI.Chat.Commands.Promote));

            var remove = typeof(AI.Chat.Commands.Remove);
            services.AddTransient(remove);
            if (diagnostics)
            {
                remove = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(remove);
                services.AddTransient(remove);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(remove));
            commandOverrides.Add(remove, nameof(AI.Chat.Commands.Remove));

            var seek = typeof(AI.Chat.Commands.Seek);
            var seekHistory = historyType;
            services.AddTransient(seek,
                serviceProvider =>
                {
                    var history = (AI.Chat.IHistory)serviceProvider
                        .GetRequiredService(seekHistory);

                    return new AI.Chat.Commands.Seek(
                        history);
                });
            if (diagnostics)
            {
                seek = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(seek);
                services.AddTransient(seek);
            }
            seek = typeof(AI.Chat.Commands.ThreadSafeRead<>)
                .MakeGenericType(seek);
            services.AddTransient(seek,
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService(seek.GenericTypeArguments[0]);
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("bot");

                    return System.Activator.CreateInstance(seek,
                        command,
                        scope);
                });
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(seek));
            commandOverrides.Add(seek, nameof(AI.Chat.Commands.Seek));

            var tag = typeof(AI.Chat.Commands.Tag);
            services.AddTransient(tag);
            if (diagnostics)
            {
                tag = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(tag);
                services.AddTransient(tag);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(tag));
            commandOverrides.Add(tag, nameof(AI.Chat.Commands.Tag));

            var timeout = typeof(AI.Chat.Commands.Timeout);
            services.AddTransient(timeout);
            if (diagnostics)
            {
                timeout = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(timeout);
                services.AddTransient(timeout);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(timeout));
            commandOverrides.Add(timeout, nameof(AI.Chat.Commands.Timeout));

            var unban = typeof(AI.Chat.Commands.Unban);
            services.AddTransient(unban);
            if (diagnostics)
            {
                unban = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(unban);
                services.AddTransient(unban);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(unban));
            commandOverrides.Add(unban, nameof(AI.Chat.Commands.Unban));

            var unmod = typeof(AI.Chat.Commands.Unmod);
            services.AddTransient(unmod);
            if (diagnostics)
            {
                unmod = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(unmod);
                services.AddTransient(unmod);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(unmod));
            commandOverrides.Add(unmod, nameof(AI.Chat.Commands.Unmod));

            var untag = typeof(AI.Chat.Commands.Untag);
            services.AddTransient(untag);
            if (diagnostics)
            {
                untag = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(untag);
                services.AddTransient(untag);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(untag));
            commandOverrides.Add(untag, nameof(AI.Chat.Commands.Untag));

            var unwelcome = typeof(AI.Chat.Commands.Unwelcome);
            services.AddTransient(unwelcome);
            if (diagnostics)
            {
                unwelcome = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(unwelcome);
                services.AddTransient(unwelcome);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(unwelcome));
            commandOverrides.Add(unwelcome, nameof(AI.Chat.Commands.Unwelcome));

            var welcome = typeof(AI.Chat.Commands.Welcome);
            services.AddTransient(welcome);
            if (diagnostics)
            {
                welcome = typeof(AI.Chat.Commands.Diagnostics.Trace<>)
                    .MakeGenericType(welcome);
                services.AddTransient(welcome);
            }
            services.AddTransient(typeof(AI.Chat.ICommand),
                serviceProvider => serviceProvider
                    .GetRequiredService(welcome));
            commandOverrides.Add(welcome, nameof(AI.Chat.Commands.Welcome));

            var commandExecutorType = typeof(AI.Chat.CommandExecutors.Slim);
            services.AddTransient(commandExecutorType,
                serviceProvider =>
                {
                    var commands = serviceProvider
                        .GetRequiredService<System.Collections.Generic.IEnumerable<AI.Chat.ICommand>>();
                    var moderator = serviceProvider
                        .GetRequiredService<AI.Chat.IModerator>();

                    return new AI.Chat.CommandExecutors.Slim(
                        commands,
                        commandOverrides,
                        moderator);
                });
            if (diagnostics)
            {
                commandExecutorType = typeof(AI.Chat.CommandExecutors.Diagnostics.Trace<>)
                    .MakeGenericType(commandExecutorType);
                services.AddTransient(commandExecutorType);
            }
            commandExecutorType = typeof(AI.Chat.CommandExecutors.Diagnostics.Log<>)
                .MakeGenericType(commandExecutorType);
            services.AddTransient(typeof(AI.Chat.ICommandExecutor),
                serviceProvider =>
                {
                    var commandExecutor = serviceProvider
                        .GetRequiredService(commandExecutorType.GenericTypeArguments[0]);
                    var logger = serviceProvider
                        .GetRequiredService(typeof(ILogger<>)
                            .MakeGenericType(commandExecutorType));

                    return System.Activator.CreateInstance(commandExecutorType,
                        commandExecutor,
                        logger);
                });

            historyType = typeof(AI.Chat.Histories.ThreadSafe<>)
                .MakeGenericType(historyType);
            services.AddSingleton(typeof(AI.Chat.IHistory),
                serviceProvider =>
                {
                    var history = serviceProvider
                        .GetRequiredService(historyType.GenericTypeArguments[0]);
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("bot");

                    return System.Activator.CreateInstance(historyType,
                        history,
                        scope);
                });

            return services;
        }
    }
}
