using AI.Chat.Extensions;

namespace AI.Chat.Bots
{
    public class Slim : IBot
    {
        private readonly IAdapter _adapter;
        private readonly IHistory _history;

        public Slim(IAdapter adapter, IHistory history)
        {
            _adapter = adapter;
            _history = history;
        }

        public async System.Threading.Tasks.Task<(System.DateTime messageKey, System.DateTime replyKey)> ReplyAsync(string username, string message)
        {
            var messageKey = _history.AddUserMessage(username, message);
            (var reply, _) = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            var replyKey = _history.AddModelMessage(reply);
            return (messageKey, replyKey);
        }
    }
}
