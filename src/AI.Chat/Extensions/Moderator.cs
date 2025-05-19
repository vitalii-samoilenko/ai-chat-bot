namespace AI.Chat.Extensions
{
    public static class Moderator
    {
        public static bool IsAllowed(this IModerator moderator, params string[] usernames)
        {
            return moderator.IsAllowed(usernames);
        }
        public static bool IsModerated(this IModerator moderator, params string[] usernames)
        {
            return moderator.IsModerated(usernames);
        }
        public static bool IsWelcomed(this IModerator moderator, params string[] usernames)
        {
            return moderator.IsWelcomed(usernames);
        }
    }
}
