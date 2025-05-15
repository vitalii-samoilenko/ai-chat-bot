namespace AI.Chat.Commands
{
    public class Ban : ICommand
    {
        private readonly IModerator _moderator;

        public Ban(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var usernames = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            _moderator.Ban(usernames);
            return usernames;
        }
    }
}
