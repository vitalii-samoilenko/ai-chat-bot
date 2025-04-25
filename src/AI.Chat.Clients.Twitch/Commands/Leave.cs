namespace AI.Chat.Commands.Twitch
{
    public class Leave : ICommand
    {
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Leave(TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _client = client;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _client.LeaveChannel(args);
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
