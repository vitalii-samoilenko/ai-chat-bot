namespace AI.Chat.Commands
{
    public class Demote : ICommand
    {
        private readonly IModerator _moderator;

        public Demote(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var usernames = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            _moderator.Demote(usernames);
            return usernames;
        }
    }
}
