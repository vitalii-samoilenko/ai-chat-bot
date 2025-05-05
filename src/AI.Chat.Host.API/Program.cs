using Microsoft.Extensions.Options;
using OpenTelemetry;
using OpenTelemetry.Metrics;
using OpenTelemetry.Trace;

var builder = WebApplication.CreateBuilder(args);

builder.Logging.ClearProviders();
builder.Logging.AddConsole();
builder.Logging.AddOpenTelemetry(logging =>
{
    logging.IncludeFormattedMessage = true;
    logging.IncludeScopes = true;
});

builder.Configuration.AddJsonFile(AI.Chat.Host.API.Helpers.Common, true);
builder.Configuration.AddJsonFile(AI.Chat.Host.API.Helpers.Moderated, true);

builder.Services.Configure<AI.Chat.Options.Bot>(
        builder.Configuration.GetSection("Chat:Bot"));
builder.Services.Configure<AI.Chat.Options.Moderator>(
        builder.Configuration.GetSection("Chat:Moderator"));
builder.Services.Configure<AI.Chat.Options.User>(
        builder.Configuration.GetSection("Chat:User"));

builder.Services.AddOpenTelemetry()
    .WithMetrics(metrics =>
    {
        metrics.AddAspNetCoreInstrumentation();
    })
    .WithTracing(tracing =>
    {
        tracing.AddAspNetCoreInstrumentation();
        tracing.AddHttpClientInstrumentation();

        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Adapters.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Bots.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.CommandExecutors.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Commands.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Filters.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Moderators.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Scopes.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Users.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Clients.Name);

        tracing.AddSource(TwitchLib.Client.Diagnostics.ActivitySources.Client.Name);
    })
    .UseOtlpExporter();

var commandOverrides = new System.Collections.Generic.Dictionary<System.Type, string>();

var adapter = builder.Configuration.GetValue<Adapters>("Chat:Adapter:Type");
var client = builder.Configuration.GetValue<Clients>("Chat:Client:Type");

builder.Services.AddTransient<AI.Chat.Scopes.Slim>();
builder.Services.AddTransient<AI.Chat.Scopes.Diagnostics.Trace<AI.Chat.Scopes.Slim>>();

switch (adapter)
{
    case Adapters.OpenAI:
        {
            builder.Services.Configure<AI.Chat.Options.OpenAI.Adapter>(
                builder.Configuration.GetSection("Chat:Adapter"));

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
            builder.Services.AddTransient<AI.Chat.Adapters.OpenAI>();
            builder.Services.AddTransient<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.OpenAI>>();
            builder.Services.AddTransient<AI.Chat.Adapters.Delayed<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.OpenAI>>>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.OpenAI.Adapter>>()
                        .Value;
                    var adapter = serviceProvider
                        .GetRequiredService<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.OpenAI>>();
                    return new AI.Chat.Adapters.Delayed<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.OpenAI>>(
                        options,
                        adapter);
                });
            builder.Services.AddTransient<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.Delayed<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.OpenAI>>>>();
            switch (client)
            {
                case Clients.Twitch:
                    {
                        builder.Services.AddTransient<AI.Chat.IAdapter, AI.Chat.Adapters.Twitch.Formatted<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.Delayed<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.OpenAI>>>>>();
                    }
                    break;
                default:
                    {
                        builder.Services.AddTransient<AI.Chat.IAdapter, AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.Delayed<AI.Chat.Adapters.Diagnostics.Trace<AI.Chat.Adapters.OpenAI>>>>();
                    }
                    break;
            }
        }
        break;
    default:
        throw new System.Exception("Adapter is not supported");
}
switch (client)
{
    case Clients.Twitch:
        {
            builder.Configuration.AddJsonFile(AI.Chat.Host.API.Helpers.OAuth, true);

            builder.Services.Configure<AI.Chat.Options.Twitch.Client>(
                builder.Configuration.GetSection("Chat:Client"));

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
                builder.Services.AddHttpClient<TwitchLib.Client.AuthClient>(
                        (serviceProvider, httpClient) =>
                        {
                            var options = serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                .Value;
                            httpClient.BaseAddress = new Uri(
                                options.Auth.Uri);
                        });
                builder.Services.AddTransient<TwitchLib.Client.Interfaces.IAuthClient, TwitchLib.Client.Diagnostics.AuthClient>();

                builder.Services.AddTransient<TwitchLib.Communication.Interfaces.IClient, TwitchLib.Communication.Clients.WebSocketClient>(
                        serviceProvider =>
                        {
                            var options = serviceProvider
                                .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                                .Value;
                            return new TwitchLib.Communication.Clients.WebSocketClient(
                                options.Communication);
                        });
                builder.Services.AddTransient<TwitchLib.Client.TwitchClient>();
                builder.Services.AddKeyedSingleton<TwitchLib.Client.Interfaces.ITwitchClient, TwitchLib.Client.Diagnostics.TwitchClient>("user");
                builder.Services.AddKeyedSingleton<TwitchLib.Client.Interfaces.ITwitchClient, TwitchLib.Client.Diagnostics.TwitchClient>("moderator");
            }
            builder.Services.AddKeyedSingleton<AI.Chat.IScope, AI.Chat.Scopes.Diagnostics.Trace<AI.Chat.Scopes.Slim>>("user");
            builder.Services.AddTransient<AI.Chat.Clients.Twitch>(
                serviceProvider =>
                {
                    var userOptions = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.User>>()
                        .Value;
                    var clientOptions = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                        .Value;

                    var logger = serviceProvider
                        .GetRequiredService<ILogger<AI.Chat.Clients.Twitch>>();

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
                    var user = serviceProvider
                        .GetRequiredService<AI.Chat.IUser>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("user");

                    return new AI.Chat.Clients.Twitch(
                        userOptions,
                        clientOptions,

                        logger,

                        authClient,
                        userClient,
                        moderatorClient,

                        commandExecutor,
                        moderator,
                        user,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.Clients.ITwitch, AI.Chat.Clients.Diagnostics.Twitch>();

            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Cheerful>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                        .Value;

                    return new AI.Chat.Commands.Twitch.Cheerful(
                        options);
                });
            builder.Services.AddTransient<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Cheerful>>();
            builder.Services.AddTransient<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Cheerful>>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Cheerful>>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("user");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Cheerful>>(
                        command,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Cheerful>>>>();
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Delay>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                        .Value;

                    return new AI.Chat.Commands.Twitch.Delay(
                        options);
                });
            builder.Services.AddTransient<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Delay>>();
            builder.Services.AddTransient<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Delay>>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Delay>>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("user");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Delay>>(
                        command,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Delay>>>>();
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Find>(
                serviceProvider =>
                {
                    var bot = serviceProvider
                        .GetRequiredService<AI.Chat.IBot>();
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("moderator");

                    return new AI.Chat.Commands.Twitch.Find(
                        bot,
                        client);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Find>>();
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Get>(
                serviceProvider =>
                {
                    var bot = serviceProvider
                        .GetRequiredService<AI.Chat.IBot>();
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("moderator");

                    return new AI.Chat.Commands.Twitch.Get(
                        bot,
                        client);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Get>>();
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Join>(
                serviceProvider =>
                {
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("user");

                    return new AI.Chat.Commands.Twitch.Join(
                        client);
                });
            builder.Services.AddTransient<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Join>>();
            builder.Services.AddTransient<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Join>>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Join>>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("user");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Join>>(
                        command,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Join>>>>();
            builder.Services.AddTransient<AI.Chat.Commands.Twitch.Leave>(
                serviceProvider =>
                {
                    var client = serviceProvider
                        .GetRequiredKeyedService<TwitchLib.Client.Interfaces.ITwitchClient>("user");

                    return new AI.Chat.Commands.Twitch.Leave(
                        client);
                });
            builder.Services.AddTransient<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Leave>>();
            builder.Services.AddTransient<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Leave>>>(
                serviceProvider =>
                {
                    var command = serviceProvider
                        .GetRequiredService<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Leave>>();
                    var scope = serviceProvider
                        .GetRequiredKeyedService<AI.Chat.IScope>("user");

                    return new AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Leave>>(
                        command,
                        scope);
                });
            builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Leave>>>>();

            commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Cheerful>>>), typeof(AI.Chat.Commands.Twitch.Cheerful).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Delay>>>), typeof(AI.Chat.Commands.Twitch.Delay).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Find>), typeof(AI.Chat.Commands.Twitch.Find).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Get>), typeof(AI.Chat.Commands.Twitch.Get).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Join>>>), typeof(AI.Chat.Commands.Twitch.Join).Name);
            commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.ThreadSafe<AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Twitch.Leave>>>), typeof(AI.Chat.Commands.Twitch.Leave).Name);

            builder.Services.AddHostedService<AI.Chat.Host.API.Services.Twitch>(
                serviceProvider =>
                {
                    var options = serviceProvider
                        .GetRequiredService<IOptions<AI.Chat.Options.Twitch.Client>>()
                        .Value;
                    var client = serviceProvider
                        .GetRequiredService<AI.Chat.Clients.ITwitch>();

                    return new AI.Chat.Host.API.Services.Twitch(
                        options,
                        client);
                });
        }
        break;
    default:
        throw new System.Exception("Client is not supported");
}

builder.Services.AddTransient<System.Collections.Generic.IEnumerable<AI.Chat.IFilter>>(
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
                        filter = new AI.Chat.Filters.Diagnostics.Trace<AI.Chat.Filters.Length>(
                            new AI.Chat.Filters.Length(
                                filterOptions.Prompt,
                                int.Parse(filterOptions.Args[0])));
                    }
                    break;
                case AI.Chat.Options.FilterType.Regex:
                    {
                        filter = new AI.Chat.Filters.Diagnostics.Trace<AI.Chat.Filters.Regex>(
                            new AI.Chat.Filters.Regex(
                                filterOptions.Prompt,
                                filterOptions.Args[0]));
                    }
                    break;
                default:
                    throw new System.Exception("Filter is not supported");
            }
            filters.Add(filter);
        }
        return filters;
    });

builder.Services.AddKeyedSingleton<AI.Chat.IScope, AI.Chat.Scopes.Diagnostics.Trace<AI.Chat.Scopes.Slim>>("bot");
builder.Services.AddKeyedSingleton<AI.Chat.IScope, AI.Chat.Scopes.Diagnostics.Trace<AI.Chat.Scopes.Slim>>("moderator");

builder.Services.AddSingleton<AI.Chat.Bots.Filtered>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Bot>>()
            .Value;
        var adapter = serviceProvider
            .GetRequiredService<AI.Chat.IAdapter>();
        var filters = serviceProvider
            .GetRequiredService<System.Collections.Generic.IEnumerable<AI.Chat.IFilter>>();

        return new AI.Chat.Bots.Filtered(
            options,
            adapter,
            filters);
    });
builder.Services.AddSingleton<AI.Chat.Bots.Diagnostics.Trace<AI.Chat.Bots.Filtered>>();
builder.Services.AddSingleton<AI.Chat.Bots.ThreadSafe<AI.Chat.Bots.Diagnostics.Trace<AI.Chat.Bots.Filtered>>>(
    serviceProvider =>
    {
        var bot = serviceProvider
            .GetRequiredService<AI.Chat.Bots.Diagnostics.Trace<AI.Chat.Bots.Filtered>>();
        var scope = serviceProvider
            .GetRequiredKeyedService<AI.Chat.IScope>("bot");

        return new AI.Chat.Bots.ThreadSafe<AI.Chat.Bots.Diagnostics.Trace<AI.Chat.Bots.Filtered>>(
            bot,
            scope);
    });
builder.Services.AddSingleton<AI.Chat.IBot, AI.Chat.Bots.Diagnostics.Trace<AI.Chat.Bots.ThreadSafe<AI.Chat.Bots.Diagnostics.Trace<AI.Chat.Bots.Filtered>>>>();

builder.Services.AddSingleton<AI.Chat.Moderators.Slim>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Moderator>>()
            .Value;

        return new AI.Chat.Moderators.Slim(
            options);
    });
builder.Services.AddSingleton<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>();
builder.Services.AddSingleton<AI.Chat.Moderators.API.Persistent<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.Moderator>>()
            .Value;
        var moderator = serviceProvider
            .GetRequiredService<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>();

        return new AI.Chat.Moderators.API.Persistent<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>(
            options,
            moderator);
    });
builder.Services.AddSingleton<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.API.Persistent<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>>>();
builder.Services.AddSingleton<AI.Chat.Moderators.ThreadSafe<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.API.Persistent<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>>>>(
    serviceProvider =>
    {
        var moderator = serviceProvider
            .GetRequiredService<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.API.Persistent<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>>>();
        var scope = serviceProvider
            .GetRequiredKeyedService<AI.Chat.IScope>("moderator");

        return new AI.Chat.Moderators.ThreadSafe<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.API.Persistent<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>>>(
            moderator,
            scope);
    });
builder.Services.AddSingleton<AI.Chat.IModerator, AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.ThreadSafe<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.API.Persistent<AI.Chat.Moderators.Diagnostics.Trace<AI.Chat.Moderators.Slim>>>>>>();

builder.Services.AddSingleton<AI.Chat.Users.Slim>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.User>>()
            .Value;
        var moderator = serviceProvider
            .GetRequiredService<AI.Chat.IModerator>();
        var bot = serviceProvider
            .GetRequiredService<AI.Chat.IBot>();

        return new AI.Chat.Users.Slim(
            options,
            moderator,
            bot);
    });
builder.Services.AddSingleton<AI.Chat.IUser, AI.Chat.Users.Diagnostics.Trace<AI.Chat.Users.Slim>>();

builder.Services.AddTransient<AI.Chat.Commands.Allow>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Allow>>();
builder.Services.AddTransient<AI.Chat.Commands.Ban>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Ban>>();
builder.Services.AddTransient<AI.Chat.Commands.Demote>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Demote>>();
builder.Services.AddTransient<AI.Chat.Commands.Deny>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Deny>>();
builder.Services.AddTransient<AI.Chat.Commands.Instruct>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Instruct>>();
builder.Services.AddTransient<AI.Chat.Commands.Mod>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Mod>>();
builder.Services.AddTransient<AI.Chat.Commands.Promote>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Promote>>();
builder.Services.AddTransient<AI.Chat.Commands.Remove>(
    serviceProvider =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Options.User>>()
            .Value;
        var bot = serviceProvider
            .GetRequiredService<AI.Chat.IBot>();

        return new AI.Chat.Commands.Remove(
            options,
            bot);
    });
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Remove>>();

builder.Services.AddTransient<AI.Chat.Commands.Timeout>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Timeout>>();
builder.Services.AddTransient<AI.Chat.Commands.Unban>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Unban>>();
builder.Services.AddTransient<AI.Chat.Commands.Unmod>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Unmod>>();
builder.Services.AddTransient<AI.Chat.Commands.Unwelcome>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Unwelcome>>();
builder.Services.AddTransient<AI.Chat.Commands.Welcome>();
builder.Services.AddTransient<AI.Chat.ICommand, AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Welcome>>();

commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Allow>), typeof(AI.Chat.Commands.Allow).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Ban>), typeof(AI.Chat.Commands.Ban).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Demote>), typeof(AI.Chat.Commands.Demote).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Deny>), typeof(AI.Chat.Commands.Deny).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Instruct>), typeof(AI.Chat.Commands.Instruct).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Mod>), typeof(AI.Chat.Commands.Mod).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Promote>), typeof(AI.Chat.Commands.Promote).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Remove>), typeof(AI.Chat.Commands.Remove).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Timeout>), typeof(AI.Chat.Commands.Timeout).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Unban>), typeof(AI.Chat.Commands.Unban).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Unmod>), typeof(AI.Chat.Commands.Unmod).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Unwelcome>), typeof(AI.Chat.Commands.Unwelcome).Name);
commandOverrides.Add(typeof(AI.Chat.Commands.Diagnostics.Trace<AI.Chat.Commands.Welcome>), typeof(AI.Chat.Commands.Welcome).Name);

builder.Services.AddTransient<AI.Chat.CommandExecutors.Slim>(
    serviceProvider =>
    {
        var commands = serviceProvider
            .GetRequiredService<System.Collections.Generic.IEnumerable<AI.Chat.ICommand>>();

        return new AI.Chat.CommandExecutors.Slim(
            commands,
            commandOverrides);
    });
builder.Services.AddTransient<AI.Chat.ICommandExecutor, AI.Chat.CommandExecutors.Diagnostics.Trace<AI.Chat.CommandExecutors.Slim>>();

builder.Services.AddControllers();
builder.Services.AddOpenApi();
builder.Services.AddHealthChecks();

var app = builder.Build();

if (app.Environment.IsDevelopment())
{
    app.MapOpenApi();
}
app.MapControllers();
app.MapHealthChecks("/health");

await app.RunAsync();

enum Adapters
{
    OpenAI
}
enum Clients
{
    Twitch
}
