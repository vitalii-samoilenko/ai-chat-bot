namespace AI.Chat.Commands.Twitch
{
    public class Join : ICommand
    {
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Join(TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _client = client;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (!string.IsNullOrWhiteSpace(args))
            {
                _client.JoinChannel(args);
                yield return true.ToString();
            }
        }
    }
}
