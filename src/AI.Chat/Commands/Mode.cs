namespace AI.Chat.Commands
{
    public class Mode : ICommand
    {
        private readonly IModerator _moderator;

        public Mode(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            if (System.Enum.TryParse(args, true, out Options.ModeratorMode mode))
            {
                _moderator.SetMode(mode);
            }
            return System.Threading.Tasks.Task.CompletedTask;
        }
    }
}
