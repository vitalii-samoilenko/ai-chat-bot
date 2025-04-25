namespace AI.Chat.Commands
{
    public class Unmod : ICommand
    {
        private readonly IModerator _moderator;

        public Unmod(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Unmoderate(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
