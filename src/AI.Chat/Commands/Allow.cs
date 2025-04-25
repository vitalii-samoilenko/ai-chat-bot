namespace AI.Chat.Commands
{
    public class Allow : ICommand
    {
        private readonly IModerator _moderator;

        public Allow(IModerator moderator)
        {
            _moderator = moderator;
        }

        public async System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            var callback = args == "all"
                ? _moderator.AllowAll()
                : _moderator.Allow(args.Split(' '));
            await callback()
                .ConfigureAwait(false);
        }
    }
}
