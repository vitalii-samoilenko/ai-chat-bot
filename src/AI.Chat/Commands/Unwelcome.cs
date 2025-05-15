namespace AI.Chat.Commands
{
    public class Unwelcome : ICommand
    {
        private readonly IModerator _moderator;

        public Unwelcome(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var usernames = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            _moderator.Unwelcome(usernames);
            return usernames;
        }
    }
}
