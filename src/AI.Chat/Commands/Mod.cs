namespace AI.Chat.Commands
{
    public class Mod : ICommand
    {
        private readonly IModerator _moderator;

        public Mod(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Moderate(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
