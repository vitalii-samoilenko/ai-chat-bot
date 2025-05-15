namespace AI.Chat.Commands
{
    public class Unmod : ICommand
    {
        private readonly IModerator _moderator;

        public Unmod(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var usernames = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            _moderator.Unmoderate(usernames);
            return usernames;
        }
    }
}
