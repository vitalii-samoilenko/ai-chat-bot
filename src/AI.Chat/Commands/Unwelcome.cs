namespace AI.Chat.Commands
{
    public class Unwelcome : ICommand
    {
        private readonly IModerator _moderator;

        public Unwelcome(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Unwelcome(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
