using Microsoft.Extensions.Logging;

namespace AI.Chat.Clients
{
    public class Twitch
    {
        private readonly Options.User _userOptions;
        private readonly Options.Twitch.Client _clientOptions;

        private readonly ILogger<Twitch> _logger;

        private readonly TwitchLib.Client.Interfaces.IAuthClient _authClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _userClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _moderatorClient;

        private readonly AI.Chat.ICommandExecutor _commandExecutor;
        private readonly AI.Chat.IModerator _moderator;
        private readonly AI.Chat.IUser _user;
        private readonly AI.Chat.IScope _scope;

        private readonly System.Collections.Generic.HashSet<string> _greeted;

        public Twitch(
                Options.User userOptions,
                Options.Twitch.Client clientOptions,

                ILogger<Twitch> logger,

                TwitchLib.Client.Interfaces.IAuthClient authClient,
                TwitchLib.Client.Interfaces.ITwitchClient userClient,
                TwitchLib.Client.Interfaces.ITwitchClient moderatorClient,

                AI.Chat.ICommandExecutor commandExecutor,
                AI.Chat.IModerator moderator,
                AI.Chat.IUser user,
                AI.Chat.IScope scope)
        {
            _userOptions = userOptions;
            _clientOptions = clientOptions;

            _logger = logger;

            _authClient = authClient;
            _userClient = userClient;
            _moderatorClient = moderatorClient;

            _commandExecutor = commandExecutor;
            _moderator = moderator;
            _user = user;
            _scope = scope;

            _greeted = new System.Collections.Generic.HashSet<string>();
        }

        public async System.Threading.Tasks.Task StartAsync()
        {
            await _commandExecutor.ExecuteAsync(
                    nameof(Commands.Remove),
                    "all")
                .ConfigureAwait(false);

            if (string.IsNullOrWhiteSpace(_clientOptions.Auth.AccessToken)
                || !await _authClient.ValidateTokenAsync(_clientOptions.Auth.AccessToken)
                    .ConfigureAwait(false))
            {
                _logger.LogWarning("Access token is invalid");

                var scopesBuilder = new System.Text.StringBuilder();
                foreach (var scope in _clientOptions.Auth.Scopes)
                {
                    scopesBuilder.Append(scope);
                    scopesBuilder.Append(' ');
                }
                var scopes = scopesBuilder.ToString()
                    .Trim();

                if (string.IsNullOrWhiteSpace(_clientOptions.Auth.RefreshToken))
                {
                    _logger.LogWarning("Refresh token is missing");

                    if (string.IsNullOrWhiteSpace(_clientOptions.Auth.DeviceCode))
                    {
                        _logger.LogWarning("Device code is missing");

                        (var deviceCode, var uri) = await _authClient.RequestAccessAsync(
                                _clientOptions.Auth.ClientId,
                                scopes)
                            .ConfigureAwait(false);
                        _clientOptions.Auth.DeviceCode = deviceCode;

                        throw new System.Exception(
                            $"Grant access via {uri}");
                    }

                    (_clientOptions.Auth.AccessToken, _clientOptions.Auth.RefreshToken) = await _authClient.IssueTokenAsync(
                            _clientOptions.Auth.ClientId,
                            _clientOptions.Auth.DeviceCode,
                            scopes)
                        .ConfigureAwait(false);
                }
                else
                {
                    _clientOptions.Auth.AccessToken = await _authClient.RefreshTokenAsync(
                            _clientOptions.Auth.ClientId,
                            _clientOptions.Auth.ClientSecret,
                            _clientOptions.Auth.RefreshToken)
                        .ConfigureAwait(false);
                }
            }

            _moderatorClient.Initialize(
                new TwitchLib.Client.Models.ConnectionCredentials(
                    _userOptions.Name,
                    _clientOptions.Auth.AccessToken),
                _userOptions.Name);
            _moderatorClient.OnChatCommandReceived += async (sender, args) =>
            {
                if (!_moderator.IsModerator(args.Command.ChatMessage.Username))
                {
                    return;
                }

                _logger.LogInformation(args.Command.ChatMessage.Message);

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
                //_logger.LogInformation($"{username} joined");

                await _user.WelcomeAsync(username,
                        async (string greeting) => await _scope.ExecuteWriteAsync(
                            async () =>
                            {
                                _logger.LogInformation($"To {username}: {greeting}");

                                _userClient.SendMessage(
                                    channel,
                                    greeting);
                                await System.Threading.Tasks.Task.Delay(
                                        _clientOptions.Delay)
                                    .ConfigureAwait(false);
                            })
                            .ConfigureAwait(false),
                        onHoldAsync)
                    .ConfigureAwait(false);
            };

            _userClient.Initialize(
                new TwitchLib.Client.Models.ConnectionCredentials(
                    _userOptions.Name,
                    _clientOptions.Auth.AccessToken),
                _clientOptions.Channel);
            _userClient.OnUserJoined += async (sender, args) =>
            {
                if (!_scope.ExecuteRead(() => _clientOptions.Welcome.Mode == Options.Twitch.WelcomeMode.OnJoin))
                {
                    return;
                }

                await welcomeAsync(args.Username, args.Channel)
                    .ConfigureAwait(false);
            };
            _userClient.OnMessageReceived += async (sender, args) =>
            {
                if (_scope.ExecuteRead(() => _clientOptions.Welcome.Mode == Options.Twitch.WelcomeMode.OnFirstMessage))
                {
                    await welcomeAsync(args.ChatMessage.Username, args.ChatMessage.Channel)
                        .ConfigureAwait(false);
                }

                if (!args.ChatMessage.Message.Contains($"@{_userOptions.Name}"))
                {
                    return;
                }

                _logger.LogInformation($"From {args.ChatMessage.Username} ({args.ChatMessage.Id}): {args.ChatMessage.Message}");

                await _user.ChatAsync(args.ChatMessage.Username, args.ChatMessage.Message,
                        async (string reply) => await _scope.ExecuteWriteAsync(
                            async () =>
                            {
                                _logger.LogInformation($"To {args.ChatMessage.Username} ({args.ChatMessage.Id}): {reply}");

                                _userClient.SendReply(
                                    args.ChatMessage.Channel,
                                    args.ChatMessage.Id,
                                    reply);
                                await System.Threading.Tasks.Task.Delay(
                                        _clientOptions.Delay)
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
