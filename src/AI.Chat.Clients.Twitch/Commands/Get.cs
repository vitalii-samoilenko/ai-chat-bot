namespace AI.Chat.Commands.Twitch
{
    public class Get : ICommand
    {
        private readonly IBot _bot;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Get(IBot bot, TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _bot = bot;
            _client = client;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            if (_bot.TryGet(args, out var message))
            {
                _client.SendMessage(_client.JoinedChannels[0], message);
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
