namespace AI.Chat.Commands
{
    public class Ban : ICommand
    {
        private readonly IModerator _moderator;

        public Ban(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Ban(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
