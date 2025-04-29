namespace AI.Chat.Commands
{
    public class Welcome : ICommand
    {
        private readonly IModerator _moderator;

        public Welcome(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Welcome(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
