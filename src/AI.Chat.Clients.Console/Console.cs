using AI.Chat.Extensions;

namespace AI.Chat.Clients
{
    public class Console
    {
        private readonly Options.Console.Client _options;

        private readonly AI.Chat.ICommandExecutor _commandExecutor;
        private readonly AI.Chat.IClient _client;
        private readonly AI.Chat.IHistory _history;

        private System.Threading.Tasks.Task _worker;

        public Console(
            Options.Console.Client options,

            AI.Chat.ICommandExecutor commandExecutor,
            AI.Chat.IClient client,
            AI.Chat.IHistory history)
        {
            _options = options;

            _commandExecutor = commandExecutor;
            _client = client;
            _history = history;
        }

        public void Start()
        {
            _worker = System.Threading.Tasks.Task.Run(async () =>
            {
                System.Func<System.DateTime, System.Threading.Tasks.Task> onAllowAsync = replyKey =>
                {
                    if (_history.TryGet(replyKey, out var reply))
                    {
                        System.Console.WriteLine(reply.Message);
                    }
                    return System.Threading.Tasks.Task.CompletedTask;
                };
                System.Func<System.DateTime, System.Threading.Tasks.Task> onHoldAsync = replyKey =>
                {
                    System.Console.WriteLine(replyKey.ToKeyString());
                    return System.Threading.Tasks.Task.CompletedTask;
                };
                await _client.WelcomeAsync(_options.Username, onAllowAsync, onHoldAsync)
                    .ConfigureAwait(false);
                for (;;)
                {
                    var line = System.Console.ReadLine();
                    if (string.IsNullOrWhiteSpace(line))
                    {
                        continue;
                    }
                    if (line.StartsWith("!", System.StringComparison.OrdinalIgnoreCase))
                    {
                        var command = line.ExtractToken(out line).Substring(1);
                        foreach (var token in _commandExecutor.Execute(_options.Username, command, line))
                        {
                            if (nameof(AI.Chat.Commands.Allow).Equals(command, System.StringComparison.OrdinalIgnoreCase))
                            {
                                await onAllowAsync(token.ParseKey())
                                    .ConfigureAwait(false);
                            }
                            else
                            {
                                System.Console.WriteLine(token);
                            }
                        }
                    }
                    else
                    {
                        await _client.ChatAsync(_options.Username, line, onAllowAsync, onHoldAsync)
                            .ConfigureAwait(false);
                    }
                }
            });
        }
    }
}
