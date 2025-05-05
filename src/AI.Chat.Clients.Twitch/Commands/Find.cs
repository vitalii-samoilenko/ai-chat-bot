namespace AI.Chat.Commands.Twitch
{
    public class Find : ICommand
    {
        private readonly IBot _bot;
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Find(IBot bot, TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _bot = bot;
            _client = client;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            System.Collections.Generic.IEnumerable<string> keys = null;
            if (args == "all")
            {
                keys = _bot.FindAll();
            }
            else
            {
                var tokens = args.Split(' ');
                if (tokens.Length != 2)
                {
                    return System.Threading.Tasks.Task.CompletedTask;
                }
                keys = _bot.Find(tokens[0], tokens[1]);
            }
            var messageBuilder = new System.Text.StringBuilder();
            foreach (var key in keys)
            {
                messageBuilder.Append('-');
                messageBuilder.Append(key);
                if (490 < messageBuilder.Length)
                {
                    messageBuilder.Append('-');
                    break;
                }
            }
            var message = 0 < messageBuilder.Length
                ? messageBuilder.Remove(0, 1)
                    .ToString()
                : null;
            if (!string.IsNullOrWhiteSpace(message))
            {
                _client.SendMessage(_client.JoinedChannels[0], message);
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
