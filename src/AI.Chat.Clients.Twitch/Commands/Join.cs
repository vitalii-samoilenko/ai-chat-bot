namespace AI.Chat.Commands.Twitch
{
    public class Join : ICommand
    {
        private readonly TwitchLib.Client.Interfaces.ITwitchClient _client;

        public Join(TwitchLib.Client.Interfaces.ITwitchClient client)
        {
            _client = client;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _client.JoinChannel(args);
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
