using Microsoft.Extensions.Logging;

namespace AI.Chat.Clients
{
    public class Twitch
    {
        private readonly Options.Bot _botOptions;
        private readonly Options.Twitch.Client _clientOptions;

        private readonly ILogger<Twitch> _logger;

        private readonly TwitchLib.Client.Interfaces.IAuthClient _authClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _userClient;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _moderatorClient;

        private readonly AI.Chat.ICommandExecutor _commandExecutor;
        private readonly AI.Chat.IModerator _moderator;
        private readonly AI.Chat.IBot _bot;
        private readonly AI.Chat.IScope _scope;

        public Twitch(
                Options.Bot botOptions,
                Options.Twitch.Client clientOptions,

                ILogger<Twitch> logger,

                TwitchLib.Client.Interfaces.IAuthClient authClient,
                TwitchLib.Client.Interfaces.ITwitchClient userClient,
                TwitchLib.Client.Interfaces.ITwitchClient moderatorClient,

                AI.Chat.ICommandExecutor commandExecutor,
                AI.Chat.IModerator moderator,
                AI.Chat.IBot bot,
                AI.Chat.IScope scope)
        {
            _botOptions = botOptions;
            _clientOptions = clientOptions;

            _logger = logger;

            _authClient = authClient;
            _userClient = userClient;
            _moderatorClient = moderatorClient;

            _commandExecutor = commandExecutor;
            _moderator = moderator;
            _bot = bot;
            _scope = scope;
        }

        public async System.Threading.Tasks.Task StartAsync()
        {
            _botOptions.Prompt = string.Format(_botOptions.Prompt, _clientOptions.Username);

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
                    _clientOptions.Username,
                    _clientOptions.Auth.AccessToken),
                _clientOptions.Username);
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

            _userClient.Initialize(
                new TwitchLib.Client.Models.ConnectionCredentials(
                    _clientOptions.Username,
                    _clientOptions.Auth.AccessToken),
                _clientOptions.Channel);
            _userClient.OnMessageReceived += async (sender, args) =>
            {
                if (!args.ChatMessage.Message.Contains($"@{_clientOptions.Username}")
                    || !_moderator.IsAllowed(_clientOptions.Username, args.ChatMessage.Username))
                {
                    return;
                }

                var message = string.Format(_clientOptions.Prompt, args.ChatMessage.Username, args.ChatMessage.Message);

                _logger.LogInformation(message);

                (var messageKey, var replyKey) = await _bot.ReplyAsync(message)
                    .ConfigureAwait(false);

                if (_bot.TryGet(replyKey, out var reply))
                {
                    _logger.LogInformation(reply);

                    System.Func<System.Threading.Tasks.Task> sendReplyAsync =
                        async () => await _scope.ExecuteWriteAsync(
                                async () =>
                                {
                                    _userClient.SendReply(
                                        args.ChatMessage.Channel,
                                        args.ChatMessage.Id,
                                        reply);
                                    await System.Threading.Tasks.Task.Delay(
                                            _clientOptions.Delay)
                                        .ConfigureAwait(false);
                                })
                            .ConfigureAwait(false);

                    if (!_moderator.IsAllowed(_clientOptions.Username, args.ChatMessage.Username))
                    {
                        _bot.Remove(messageKey, replyKey);
                        return;
                    }
                    if (_moderator.IsModerated(_clientOptions.Username, args.ChatMessage.Username))
                    {
                        _moderator.Hold(replyKey,
                        (
                            sendReplyAsync,
                            () =>
                            {
                                _bot.Remove(messageKey, replyKey);
                                return System.Threading.Tasks.Task.CompletedTask;
                            }
                        ));
                        _moderatorClient.SendMessage(
                            _moderatorClient.JoinedChannels[0],
                            $"{replyKey}: {reply}");
                        return;
                    }

                    await sendReplyAsync()
                        .ConfigureAwait(false);
                }
            };
            if (!_userClient.Connect())
            {
                throw new System.Exception(
                    "Failed to connect to Twitch user channel");
            }
        }
    }
}
