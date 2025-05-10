using AI.Chat.Adapters.Extensions.OpenAI;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using NLog.Extensions.Logging;

var builder = Host.CreateApplicationBuilder(args);

builder.Logging.ClearProviders();
builder.Logging.AddNLog();

builder.Configuration.AddJsonFile(AI.Chat.Host.Console.Constants.JsonCommon, true);
builder.Configuration.AddJsonFile(AI.Chat.Host.Console.Constants.JsonModerated, true);

builder.Services.Configure<AI.Chat.Options.Bot>(
        builder.Configuration.GetSection("Chat:Bot"));
builder.Services.Configure<AI.Chat.Options.Moderator>(
        builder.Configuration.GetSection("Chat:Moderator"));

var commandOverrides = new System.Collections.Generic.Dictionary<System.Type, string>();

var adapter = builder.Configuration.GetValue<Adapters>("Chat:Adapter:Type");
var client = builder.Configuration.GetValue<Clients>("Chat:Client:Type");

var historyType = typeof(AI.Chat.Histories.Slim);
builder.Services.AddSingleton(historyType);
builder.Services.AddSingleton<System.Collections.Generic.TimeSeries<AI.Chat.Record>>(
    serviceProvider => AI.Chat.Host.Console.Helpers.LoadLog());
historyType = typeof(AI.Chat.Histories.Console.Persistent<>)
    .MakeGenericType(historyType);
builder.Services.AddSingleton(historyType);

System.Type adapterType = null;
switch (adapter)
{
    case Adapters.OpenAI:
        {
            builder.Services.Configure<AI.Chat.Options.OpenAI.Adapter>(
                builder.Configuration.GetSection("Chat:Adapter"));
            builder.Services.AddSingleton<IOptions<AI.Chat.Options.Adapter>>(
                serviceProvider => serviceProvider
                    .GetRequiredService<IOptions<AI.Chat.Options.OpenAI.Adapter>>());

            builder.Services.AddSingleton<System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage>>(
                serviceProvider =>
                {
                    var records = serviceProvider
                        .GetRequiredService<System.Collections.Generic.TimeSeries<AI.Chat.Record>>();
                    var messages = new System.Collections.Generic.TimeSeries<global::OpenAI.Chat.ChatMessage>(
                        records.Start);
                    foreach (var entry in records.Entries)
                    {
                        messages.Add(entry.Key, entry.Value.ToChatMessage());
                    }

                    return messages;
                });
            historyType = typeof(AI.Chat.Histories.OpenAI.Tracked<>)
                .MakeGenericType(historyType);
            builder.Services.AddSingleton(historyType);

            builder.Services.AddTransient<OpenAI.Chat.ChatClient>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.OpenAI.Adapter>>()
                        .Value;

                    return new OpenAI.Chat.ChatClient(
                        options.Model,
                        new System.ClientModel.ApiKeyCredential(
                            options.ApiKey),
                        options.Client
                    );
                });

            adapterType = typeof(AI.Chat.Adapters.OpenAI);
            builder.Services.AddTransient(adapterType);
        }
        break;
    default:
        throw new System.Exception("Adapter is not supported");
}
switch (client)
{
    case Clients.Twitch:
        {
            builder.Configuration.AddJsonFile(AI.Chat.Host.Console.Constants.JsonOAuth, true);

            builder.Services.Configure<AI.Chat.Options.Twitch.Client>(
                builder.Configuration.GetSection("Chat:Client"));
            builder.Services.AddSingleton<IOptions<AI.Chat.Options.Client>>(
                serviceProvider => serviceProvider
                    .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>());

            adapterType = typeof(AI.Chat.Adapters.Twitch.Formatted<>)
                .MakeGenericType(adapterType);
            builder.Services.AddTransient(adapterType);

            if (builder.Environment.IsDevelopment())
            {
                builder.Services.AddTransient<TwitchLib.Client.Interfaces.IAuthClient, TwitchLib.Client.DummyAuthClient>();
                builder.Services.AddSingleton<TwitchLib.Client.DummyTwitchClient>();
                builder.Services.AddKeyedTransient<TwitchLib.Client.Interfaces.ITwitchClient, TwitchLib.Client.DummyTwitchClient>(
                    "user",
                    (serviceProvider, _) =>
                    {
                        var client = serviceProvider
                            .GetRequiredService<TwitchLib.Client.DummyTwitchClient>();

                        return client;
                    });
                builder.Services.AddKeyedTransient<TwitchLib.Client.Interfaces.ITwitchClient, TwitchLib.Client.DummyTwitchClient>(
                    "moderator",
                    (serviceProvider, _) =>
                    {
                        var client = serviceProvider
                            .GetRequiredService<TwitchLib.Client.DummyTwitchClient>();

                        return client;
                    });
            }
            else
            {
                builder.Services.AddHttpClient<TwitchLib.Client.Interfaces.IAuthClient, TwitchLib.Client.AuthClient>(
                        (serviceProvider, httpClient) =>
                        {
                            var options = serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                .Value;

                            httpClient.BaseAddress = new Uri(
                                options.Auth.Uri);
                        });

                builder.Services.AddTransient<TwitchLib.Communication.Interfaces.IClient, TwitchLib.Communication.Clients.WebSocketClient>(
                        serviceProvider =>
                        {
                            var options = serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                .Value;

                            return new TwitchLib.Communication.Clients.WebSocketClient(
                                options.Communication);
                        });
                builder.Services.AddKeyedSingleton<TwitchLib.Client.Interfaces.ITwitchClient, TwitchLib.Client.TwitchClient>("user");
                builder.Services.AddKeyedSingleton<TwitchLib.Client.Interfaces.ITwitchClient, TwitchLib.Client.TwitchClient>("moderator");
            }
            builder.Services.AddKeyedSingleton<AI.Chat.IScope, AI.Chat.Scopes.Slim>("client");
            builder.Services.AddTransient<AI.Chat.Clients.Twitch>(
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
                    var moderator = serviceProvider
                        .GetRequiredService<AI.Chat.IModerator>();
                    var client = serviceProvider
                        .GetRequiredService<AI.Chat.IClient>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("client");

                    return new AI.Chat.Clients.Twitch(
                        options,

                        authClient,
                        userClient,
                        moderatorClient,

                        commandExecutor,
                        moderator,
                        client,
                        scope);
                });

            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Cheerful>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                        .Value;

                    return new AI.Chat.Commands.Twitch.Cheerful(
                        options);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Cheerful>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Twitch.Cheerful>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("client");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Cheerful>(
                        command,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Delay>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                        .Value;

                    return new AI.Chat.Commands.Twitch.Delay(
                        options);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Delay>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Twitch.Delay>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("client");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Delay>(
                        command,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Twitch.Find>(
                serviceProvider =>
                {
                    var history = serviceProvider
                        .GetRequiredService<AI.Chat.IHistory>();
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("moderator");

                    return new AI.Chat.Commands.Twitch.Find(
                        history,
                        client);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Twitch.Get>(
                serviceProvider =>
                {
                    var history = serviceProvider
                        .GetRequiredService<AI.Chat.IHistory>();
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("moderator");

                    return new AI.Chat.Commands.Twitch.Get(
                        history,
                        client);
                });
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Join>(
                serviceProvider =>
                {
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("user");

                    return new AI.Chat.Commands.Twitch.Join(
                        client);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Join>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Twitch.Join>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("client");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Join>(
                        command,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Leave>(
                serviceProvider =>
                {
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("user");

                    return new AI.Chat.Commands.Twitch.Leave(
                        client);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Leave>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Twitch.Leave>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("client");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Leave>(
                        command,
                        scope);
                });

            commandOverrides.Add(typeof(AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Cheerful>), typeof(AI.Chat.Commands.Twitch.Cheerful).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Delay>), typeof(AI.Chat.Commands.Twitch.Delay).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Join>), typeof(AI.Chat.Commands.Twitch.Join).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Twitch.Leave>), typeof(AI.Chat.Commands.Twitch.Leave).Name);

            builder.Services.AddHostedService<AI.Chat.Host.Console.Services.Twitch>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                        .Value;
                    var client = serviceProvider
                        .GetRequiredService<AI.Chat.Clients.Twitch>();

                    return new AI.Chat.Host.Console.Services.Twitch(
                        options,
                        client);
                });
        }
        break;
    default:
        throw new System.Exception("Client is not supported");
}

var limitedAdapterType = typeof(AI.Chat.Adapters.Limited<>)
    .MakeGenericType(adapterType);
var limitedHistoryType = historyType;
builder.Services.AddTransient(limitedAdapterType,
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Adapter>>()
            .Value;
        var adapter = serviceProvider
            .GetRequiredService(limitedAdapterType.GenericTypeArguments[0]);
        var history = serviceProvider
            .GetRequiredService(limitedHistoryType);

        return System.Activator.CreateInstance(limitedAdapterType,
            options,
            adapter,
            history);
    });
adapterType = limitedAdapterType;
var delayedAapterType = typeof(AI.Chat.Adapters.Delayed<>)
    .MakeGenericType(adapterType);
builder.Services.AddTransient(delayedAapterType,
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Adapter>>()
            .Value;
        var adapter = serviceProvider
            .GetRequiredService(delayedAapterType.GenericTypeArguments[0]);

        return System.Activator.CreateInstance(delayedAapterType,
            options,
            adapter);
    });
adapterType = delayedAapterType;
var logAdapterType = typeof(AI.Chat.Adapters.Diagnostics.Log<>)
    .MakeGenericType(adapterType);
builder.Services.AddTransient(typeof(AI.Chat.IAdapter), logAdapterType);
adapterType = logAdapterType;

builder.Services.AddTransient<AI.Chat.IFilter>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Bot>>()
            .Value;
        var filters = new System.Collections.Generic.List<AI.Chat.IFilter>();
        foreach (var filterOptions in options.Filters)
        {
            AI.Chat.IFilter filter = null;
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
            filters.Add(filter);
        }

        return new AI.Chat.Filters.Composite(filters);
    });

builder.Services.AddKeyedSingleton<AI.Chat.IScope, AI.Chat.Scopes.Slim>("bot");
builder.Services.AddKeyedSingleton<AI.Chat.IScope, AI.Chat.Scopes.Slim>("moderator");

var filteredHistoryType = historyType;
builder.Services.AddSingleton<AI.Chat.Bots.Filtered>(
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
            .GetRequiredService(filteredHistoryType);

        return new AI.Chat.Bots.Filtered(
            options,
            adapter,
            filter,
            history);
    });
builder.Services.AddSingleton<AI.Chat.Bots.ThreadSafe<AI.Chat.Bots.Filtered>>(
    serviceProvider =>
    {
        var bot = serviceProvider
            .GetRequiredService<AI.Chat.Bots.Filtered>();
        var scope = serviceProvider
            .GetRequiredKeyedService<AI.Chat.IScope>("bot");

        return new AI.Chat.Bots.ThreadSafe<AI.Chat.Bots.Filtered>(
            bot,
            scope);
    });
builder.Services.AddSingleton<AI.Chat.IBot, AI.Chat.Bots.Diagnostics.Log<AI.Chat.Bots.ThreadSafe<AI.Chat.Bots.Filtered>>>();

builder.Services.AddSingleton<AI.Chat.Moderators.Slim>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Moderator>>()
            .Value;

        return new AI.Chat.Moderators.Slim(
            options);
    });
builder.Services.AddSingleton<AI.Chat.Moderators.Console.Persistent<AI.Chat.Moderators.Slim>>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Moderator>>()
            .Value;
        var moderator = serviceProvider
            .GetRequiredService<AI.Chat.Moderators.Slim>();

        return new AI.Chat.Moderators.Console.Persistent<AI.Chat.Moderators.Slim>(
            options,
            moderator);
    });
builder.Services.AddSingleton<AI.Chat.IModerator, AI.Chat.Moderators.ThreadSafe<AI.Chat.Moderators.Console.Persistent<AI.Chat.Moderators.Slim>>>(
    serviceProvider =>
    {
        var moderator = serviceProvider
            .GetRequiredService<AI.Chat.Moderators.Console.Persistent<AI.Chat.Moderators.Slim>>();
        var scope = serviceProvider
            .GetRequiredKeyedService<AI.Chat.IScope>("moderator");

        return new AI.Chat.Moderators.ThreadSafe<AI.Chat.Moderators.Console.Persistent<AI.Chat.Moderators.Slim>>(
            moderator,
            scope);
    });

var threadSafeHistoryType = typeof(AI.Chat.Histories.ThreadSafe<>)
    .MakeGenericType(historyType);
builder.Services.AddSingleton(typeof(AI.Chat.IHistory),
    serviceProvider =>
    {
        var history = serviceProvider
            .GetRequiredService(threadSafeHistoryType.GenericTypeArguments[0]);
        var scope = serviceProvider
            .GetRequiredKeyedService<AI.Chat.IScope>("bot");

        return System.Activator.CreateInstance(threadSafeHistoryType,
            history,
            scope);
    });
historyType = threadSafeHistoryType;

builder.Services.AddSingleton<AI.Chat.IClient, AI.Chat.Clients.Slim>(
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

builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Allow>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Ban>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Demote>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Deny>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Instruct>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Mod>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Promote>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Remove>();

builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Timeout>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Unban>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Unmod>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Unwelcome>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Welcome>();

builder.Services.AddTransient<AI.Chat.ICommandExecutor, AI.Chat.CommandExecutors.Slim>(
    serviceProvider =>
    {
        var commands = serviceProvider
            .GetRequiredService<System.Collections.Generic.IEnumerable<AI.Chat.ICommand>>();

        return new AI.Chat.CommandExecutors.Slim(
            commands,
            commandOverrides);
    });

using var host = builder.Build();

await host.RunAsync();

enum Adapters
{
    OpenAI
}
enum Clients
{
    Twitch
}
