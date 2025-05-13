using AI.Chat.Extensions;

namespace AI.Chat.Commands.Twitch
{
    public class Find : ICommand
    {
        private readonly IHistory _history;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Find(IHistory history, TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _history = history;
            _client = client;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            var fromKey = System.DateTime.MinValue;
            var toKey = System.DateTime.MaxValue;
            if (args.StartsWith(Constants.ArgsAll))
            {
                args = args.Substring(Constants.ArgsAll.Length);
            }
            else
            {
                var previous = 0;
                var next = args.IndexOf(' ', previous);
                if (next < 0
                    || !args.Substring(previous, next - previous).TryParseKey(out fromKey))
                {
                    return System.Threading.Tasks.Task.CompletedTask;
                }
                previous = next + 1;
                next = args.IndexOf(' ', previous);
                if (next < 0)
                {
                    next = args.Length;
                }
                if (!args.Substring(previous, next - previous).TryParseKey(out toKey))
                {
                    return System.Threading.Tasks.Task.CompletedTask;
                }
                args = next < args.Length
                    ? args.Substring(next + 1)
                    : string.Empty;
            }
            var maxLength = 501 - Constants.HistoryKeyFormat.Length;
            var messageBuilder = new System.Text.StringBuilder();
            foreach (var key in _history.Find(fromKey, toKey, args.Split(new char[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries)))
            {
                messageBuilder.Append('-');
                messageBuilder.Append(key.ToKeyString());
                if (maxLength < messageBuilder.Length)
                {
                    messageBuilder.Append('-');
                    break;
                }
            }
            var message = 0 < messageBuilder.Length
                ? messageBuilder.Remove(0, 1).ToString()
                : null;
            if (!string.IsNullOrWhiteSpace(message))
            {
                _client.SendMessage(_client.JoinedChannels[0], message);
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
