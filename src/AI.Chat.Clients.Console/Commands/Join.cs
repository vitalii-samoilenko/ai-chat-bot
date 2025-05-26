using AI.Chat.Extensions;

namespace AI.Chat.Commands.Console
{
    public class Join : ICommand
    {
        private readonly IClient _client;
        private readonly IHistory _history;

        public Join(IClient client, IHistory history)
        {
            _client = client;
            _history = history;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (string.IsNullOrWhiteSpace(args))
            {
                yield break;
            }
            string replyMessage = null;
            _client.WelcomeAsync(args,
                replyKey =>
                {
                    if (_history.TryGet(replyKey, out var reply))
                    {
                        replyMessage = reply.Message;
                    }
                    return System.Threading.Tasks.Task.CompletedTask;
                },
                replyKey =>
                {
                    replyMessage = replyKey.ToKeyString();
                    return System.Threading.Tasks.Task.CompletedTask;
                })
                .ConfigureAwait(false)
                .GetAwaiter()
                .GetResult();
            if (!string.IsNullOrWhiteSpace(replyMessage))
            {
                yield return replyMessage;
            }
        }
    }
}
