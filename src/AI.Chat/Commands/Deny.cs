namespace AI.Chat.Commands
{
    public class Deny : ICommand
    {
        private readonly IModerator _moderator;

        public Deny(IModerator moderator)
        {
            _moderator = moderator;
        }

        public async System.Threading.Tasks.Task ExecuteAsync(string args)
        {
            var callback = args == "all"
                ? _moderator.AllowAll()
                : _moderator.Deny(args.Split(' '));
            await callback()
                .ConfigureAwait(false);
        }
    }
}
