namespace AI.Chat.Commands
{
    public class Promote : ICommand
    {
        private readonly IModerator _moderator;

        public Promote(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Promote(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
