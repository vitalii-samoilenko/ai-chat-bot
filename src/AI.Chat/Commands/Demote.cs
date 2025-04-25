namespace AI.Chat.Commands
{
    public class Demote : ICommand
    {
        private readonly IModerator _moderator;

        public Demote(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            _moderator.Demote(args.Split(' '));
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
