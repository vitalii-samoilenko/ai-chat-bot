namespace AI.Chat.Commands.Twitch
{
    public class Leave : ICommand
    {
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Leave(TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _client = client;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            if (!string.IsNullOrWhiteSpace(args))
            {
                _client.LeaveChannel(args);
                yield return true.ToString();
            }
        }
    }
}
