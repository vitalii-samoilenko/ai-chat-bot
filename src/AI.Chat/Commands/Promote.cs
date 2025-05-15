namespace AI.Chat.Commands
{
    public class Promote : ICommand
    {
        private readonly IModerator _moderator;

        public Promote(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var usernames = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            _moderator.Promote(usernames);
            return usernames;
        }
    }
}
