namespace AI.Chat.Moderators.Diagnostics
{
    public class Trace<TModerator> : IModerator
        where TModerator : IModerator
    {
        private static string ModeratorName = $"{typeof(TModerator).Namespace}.{typeof(TModerator).Name}";

        private readonly IModerator _moderator;

        public Trace(TModerator moderator)
        {
            _moderator = moderator;
        }

        public bool IsModerator(string username)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(IsModerator)}"))
            {
                return _moderator.IsModerator(username);
            }
        }
        public bool IsModerated(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(IsModerated)}"))
            {
                return _moderator.IsModerated(usernames);
            }
        }
        public bool IsAllowed(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(IsAllowed)}"))
            {
                return _moderator.IsAllowed(usernames);
            }
        }
        public bool IsWelcomed(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(IsWelcomed)}"))
            {
                return _moderator.IsWelcomed(usernames);
            }
        }

        public string[] Ban(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Ban)}"))
            {
                return _moderator.Ban(usernames);
            }
        }
        public string[] Unban(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Unban)}"))
            {
                return _moderator.Unban(usernames);
            }
        }
        public (string username, System.DateTime until)[] Timeout(params (string username, System.TimeSpan timeout)[] args)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Timeout)}"))
            {
                return _moderator.Timeout(args);
            }
        }
        public string[] Moderate(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Moderate)}"))
            {
                return _moderator.Moderate(usernames);
            }
        }
        public string[] Unmoderate(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Unmoderate)}"))
            {
                return _moderator.Unmoderate(usernames);
            }
        }
        public string[] Promote(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Promote)}"))
            {
                return _moderator.Promote(usernames);
            }
        }
        public string[] Demote(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Demote)}"))
            {
                return _moderator.Demote(usernames);
            }
        }
        public string[] Welcome(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Welcome)}"))
            {
                return _moderator.Welcome(usernames);
            }
        }
        public string[] Unwelcome(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Unwelcome)}"))
            {
                return _moderator.Unwelcome(usernames);
            }
        }

        public bool Greet(string username)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Greet)}"))
            {
                return _moderator.Greet(username);
            }
        }
    }
}
