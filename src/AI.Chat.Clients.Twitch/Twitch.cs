using AI.Chat.Extensions;

namespace AI.Chat.Clients
{
    public class Twitch
    {
        private const int MaxMessageLength = 500;

        private readonly Options.Twitch.Client _options;

        private readonly TwitchLib.Client.Interfaces.IAuthClient _authClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _userClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _moderatorClient;

        private readonly AI.Chat.ICommandExecutor _commandExecutor;
        private readonly AI.Chat.IClient _client;
        private readonly AI.Chat.IHistory _history;
        private readonly AI.Chat.IScope _scope;

        public Twitch(
            Options.Twitch.Client options,

            TwitchLib.Client.Interfaces.IAuthClient authClient,
            TwitchLib.Client.Interfaces.ITwitchClient userClient,
            TwitchLib.Client.Interfaces.ITwitchClient moderatorClient,

            AI.Chat.ICommandExecutor commandExecutor,
            AI.Chat.IClient client,
            AI.Chat.IHistory history,
            AI.Chat.IScope scope)
        {
            _options = options;

            _authClient = authClient;
            _userClient = userClient;
            _moderatorClient = moderatorClient;

            _commandExecutor = commandExecutor;
            _client = client;
            _history = history;
            _scope = scope;
        }

        public async System.Threading.Tasks.Task StartAsync()
        {
            if (string.IsNullOrWhiteSpace(_options.Auth.AccessToken)
                || !await _authClient.ValidateTokenAsync(_options.Auth.AccessToken)
                    .ConfigureAwait(false))
            {
                var scopesBuilder = new System.Text.StringBuilder();
                foreach (var scope in _options.Auth.Scopes)
                {
                    scopesBuilder.Append(' ')
                        .Append(scope);
                }
                var scopes = scopesBuilder.Remove(0, 1)
                    .ToString();

                if (string.IsNullOrWhiteSpace(_options.Auth.RefreshToken))
                {
                    if (string.IsNullOrWhiteSpace(_options.Auth.DeviceCode))
                    {
                        (var deviceCode, var uri) = await _authClient.RequestAccessAsync(
                                _options.Auth.ClientId,
                                scopes)
                            .ConfigureAwait(false);
                        _options.Auth.DeviceCode = deviceCode;

                        throw new System.Exception(
                            $"Grant access via {uri}");
                    }

                    (_options.Auth.AccessToken, _options.Auth.RefreshToken) = await _authClient.IssueTokenAsync(
                            _options.Auth.ClientId,
                            _options.Auth.DeviceCode,
                            scopes)
                        .ConfigureAwait(false);
                }
                else
                {
                    _options.Auth.AccessToken = await _authClient.RefreshTokenAsync(
                            _options.Auth.ClientId,
                            _options.Auth.ClientSecret,
                            _options.Auth.RefreshToken)
                        .ConfigureAwait(false);
                }
            }

            System.Func<System.DateTime, string, System.Threading.Tasks.Task> onAllowAsync = async (replyKey, channel) =>
            {
                if (!_history.TryGet(replyKey, out var reply))
                {
                    return;
                }
                await _scope.ExecuteWriteAsync(
                    async () =>
                    {
                        _userClient.SendMessage(
                            channel,
                            reply.Message);
                        await System.Threading.Tasks.Task.Delay(
                                _options.Delay)
                            .ConfigureAwait(false);
                    })
                    .ConfigureAwait(false);
            };
            System.Func<System.DateTime, System.Threading.Tasks.Task> onHoldAsync = replyKey =>
            {
                if (!_history.TryGet(replyKey, out var reply))
                {
                    return System.Threading.Tasks.Task.CompletedTask;
                }
                var replyNotify = $"{replyKey.ToKeyString()} {reply.Message}";
                if (MaxMessageLength < replyNotify.Length)
                {
                    replyNotify = $"{replyNotify.Substring(0, MaxMessageLength - 3)}{AI.Chat.Defaults.Etc}";
                }
                _moderatorClient.SendMessage(
                    _moderatorClient.JoinedChannels[0],
                    replyNotify);
                return System.Threading.Tasks.Task.CompletedTask;
            };
            System.Func<string, string, System.Threading.Tasks.Task> welcomeAsync = async (username, channel) =>
                await _client.WelcomeAsync(username,
                        async replyKey => await onAllowAsync(replyKey, channel)
                            .ConfigureAwait(false),
                        onHoldAsync)
                    .ConfigureAwait(false);

            _moderatorClient.Initialize(
                new TwitchLib.Client.Models.ConnectionCredentials(
                    _options.Botname,
                    _options.Auth.AccessToken),
                _options.Botname);
            _moderatorClient.OnChatCommandReceived += (sender, args) =>
            {
                if (nameof(AI.Chat.Commands.Seek).Equals(args.Command.CommandText, System.StringComparison.OrdinalIgnoreCase))
                {
                    return;
                }

                var replyBuilder = new System.Text.StringBuilder();
                foreach (var token in _commandExecutor.Execute(args.Command.ChatMessage.Username, args.Command.CommandText, args.Command.ArgumentsAsString))
                {
                    replyBuilder.Append(' ')
                        .Append(token);
                    if (nameof(AI.Chat.Commands.Allow).Equals(args.Command.CommandText, System.StringComparison.OrdinalIgnoreCase))
                    {
                        onAllowAsync(token.ParseKey(), _userClient.JoinedChannels[0].Channel)
                            .ConfigureAwait(false)
                            .GetAwaiter()
                            .GetResult();
                    }
                    else if (MaxMessageLength + 1 < replyBuilder.Length)
                    {
                        break;
                    }
                }
                if (MaxMessageLength + 1 < replyBuilder.Length)
                {
                    replyBuilder.Remove(MaxMessageLength - 2, replyBuilder.Length - (MaxMessageLength - 2))
                        .Append(AI.Chat.Defaults.Etc);
                }
                
                if (0 < replyBuilder.Length)
                {
                    var reply = replyBuilder.Remove(0, 1)
                        .ToString();
                    _moderatorClient.SendReply(
                        args.Command.ChatMessage.Channel,
                        args.Command.ChatMessage.Id,
                        reply);
                }
            };
            if (!_moderatorClient.Connect())
            {
                throw new System.Exception(
                    "Failed to connect to Twitch moderator channel");
            }

            _userClient.Initialize(
                new TwitchLib.Client.Models.ConnectionCredentials(
                    _options.Botname,
                    _options.Auth.AccessToken),
                _options.Channel);
            _userClient.OnUserJoined += (sender, args) =>
            {
                if (!_scope.ExecuteRead(() => _options.Welcome.Mode == Options.Twitch.WelcomeMode.OnJoin))
                {
                    return;
                }

                welcomeAsync(args.Username, args.Channel)
                    .ConfigureAwait(false)
                    .GetAwaiter()
                    .GetResult();
            };
            _userClient.OnMessageReceived += (sender, args) =>
            {
                if (!args.ChatMessage.Message.Contains($"@{_options.Botname}"))
                {
                    if (_scope.ExecuteRead(() => _options.Welcome.Mode == Options.Twitch.WelcomeMode.OnFirstMessage))
                    {
                        welcomeAsync(args.ChatMessage.Username, args.ChatMessage.Channel)
                            .ConfigureAwait(false)
                            .GetAwaiter()
                            .GetResult();
                    }
                    return;
                }

                _client.ChatAsync(args.ChatMessage.Username, args.ChatMessage.Message,
                        async replyKey => await onAllowAsync(replyKey, args.ChatMessage.Channel)
                            .ConfigureAwait(false),
                        onHoldAsync)
                    .ConfigureAwait(false)
                    .GetAwaiter()
                    .GetResult();
            };
            if (!_userClient.Connect())
            {
                throw new System.Exception(
                    "Failed to connect to Twitch user channel");
            }
        }
    }
}
