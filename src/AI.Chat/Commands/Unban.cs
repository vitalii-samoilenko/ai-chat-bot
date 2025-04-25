namespace AI.Chat.Commands
{
    public class Unban : ICommand
    {
        private readonly IModerator _moderator;

        public Unban(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Unban(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
