using AI.Chat.Extensions;

namespace AI.Chat.Commands.Twitch
{
    public class Get : ICommand
    {
        private readonly IHistory _history;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Get(IHistory history, TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _history = history;
            _client = client;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            if (args.TryParseKey(out var key)
                && _history.TryGet(key, out var record))
            {
                _client.SendMessage(_client.JoinedChannels[0], record.Message);
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
