namespace AI.Chat.Clients
{
    public interface ITwitch
    {
        System.Threading.Tasks.Task StartAsync();
    }

    public class Twitch : ITwitch
    {
        private readonly Options.Twitch.Client _options;

        private readonly TwitchLib.Client.Interfaces.IAuthClient _authClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _userClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _moderatorClient;

        private readonly AI.Chat.ICommandExecutor _commandExecutor;
        private readonly AI.Chat.IModerator _moderator;
        private readonly AI.Chat.IClient _client;
        private readonly AI.Chat.IScope _scope;

        public Twitch(
                Options.Twitch.Client options,

                TwitchLib.Client.Interfaces.IAuthClient authClient,
                TwitchLib.Client.Interfaces.ITwitchClient userClient,
                TwitchLib.Client.Interfaces.ITwitchClient moderatorClient,

                AI.Chat.ICommandExecutor commandExecutor,
                AI.Chat.IModerator moderator,
                AI.Chat.IClient client,
                AI.Chat.IScope scope)
        {
            _options = options;

            _authClient = authClient;
            _userClient = userClient;
            _moderatorClient = moderatorClient;

            _commandExecutor = commandExecutor;
            _moderator = moderator;
            _client = client;
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
                    scopesBuilder.Append(scope);
                    scopesBuilder.Append(' ');
                }
                var scopes = scopesBuilder.ToString()
                    .Trim();

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

            _moderatorClient.Initialize(
                new TwitchLib.Client.Models.ConnectionCredentials(
                    _options.Username,
                    _options.Auth.AccessToken),
                _options.Username);
            _moderatorClient.OnChatCommandReceived += async (sender, args) =>
            {
                if (!_moderator.IsModerator(args.Command.ChatMessage.Username))
                {
                    return;
                }

                if(!await _commandExecutor.ExecuteAsync(
                            args.Command.CommandText,
                            args.Command.ArgumentsAsString)
                        .ConfigureAwait(false))
                {
                    return;
                }

                _moderatorClient.SendReply(
                    args.Command.ChatMessage.Channel,
                    args.Command.ChatMessage.Id,
                    "ack");
            };
            if (!_moderatorClient.Connect())
            {
                throw new System.Exception(
                    "Failed to connect to Twitch moderator channel");
            }

            System.Func<string, System.Threading.Tasks.Task> onHoldAsync = (warning) =>
            {
                _moderatorClient.SendMessage(
                    _moderatorClient.JoinedChannels[0],
                    warning);
                return System.Threading.Tasks.Task.CompletedTask;
            };
            System.Func<string, string, System.Threading.Tasks.Task> welcomeAsync = async (username, channel) =>
            {
                await _client.WelcomeAsync(username,
                        async (string greeting) => await _scope.ExecuteWriteAsync(
                            async () =>
                            {
                                _userClient.SendMessage(
                                    channel,
                                    greeting);
                                await System.Threading.Tasks.Task.Delay(
                                        _options.Delay)
                                    .ConfigureAwait(false);
                            })
                            .ConfigureAwait(false),
                        onHoldAsync)
                    .ConfigureAwait(false);
            };

            _userClient.Initialize(
                new TwitchLib.Client.Models.ConnectionCredentials(
                    _options.Username,
                    _options.Auth.AccessToken),
                _options.Channel);
            _userClient.OnUserJoined += async (sender, args) =>
            {
                if (!_scope.ExecuteRead(() => _options.Welcome.Mode == Options.Twitch.WelcomeMode.OnJoin))
                {
                    return;
                }

                await welcomeAsync(args.Username, args.Channel)
                    .ConfigureAwait(false);
            };
            _userClient.OnMessageReceived += async (sender, args) =>
            {
                if (_scope.ExecuteRead(() => _options.Welcome.Mode == Options.Twitch.WelcomeMode.OnFirstMessage))
                {
                    await welcomeAsync(args.ChatMessage.Username, args.ChatMessage.Channel)
                        .ConfigureAwait(false);
                }

                if (!args.ChatMessage.Message.Contains($"@{_options.Username}"))
                {
                    return;
                }

                await _client.ChatAsync(args.ChatMessage.Username, args.ChatMessage.Message.Replace("@", string.Empty),
                        async (string reply) => await _scope.ExecuteWriteAsync(
                            async () =>
                            {
                                _userClient.SendReply(
                                    args.ChatMessage.Channel,
                                    args.ChatMessage.Id,
                                    reply);
                                await System.Threading.Tasks.Task.Delay(
                                        _options.Delay)
                                    .ConfigureAwait(false);
                            })
                            .ConfigureAwait(false),
                        onHoldAsync)
                    .ConfigureAwait(false);
            };
            if (!_userClient.Connect())
            {
                throw new System.Exception(
                    "Failed to connect to Twitch user channel");
            }
        }
    }
}
