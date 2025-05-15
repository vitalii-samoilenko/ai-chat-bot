namespace AI.Chat.Commands
{
    public class Welcome : ICommand
    {
        private readonly IModerator _moderator;

        public Welcome(IModerator moderator)
        {
            _moderator = moderator;
        }

        public System.Collections.Generic.IEnumerable<string> Execute(string args)
        {
            var usernames = args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
            _moderator.Welcome(usernames);
            return usernames;
        }
    }
}
