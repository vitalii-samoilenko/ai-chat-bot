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

        public void Ban(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Ban)}"))
            {
                _moderator.Ban(usernames);
            }
        }
        public void Unban(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Unban)}"))
            {
                _moderator.Unban(usernames);
            }
        }
        public void Timeout(params (string username, System.TimeSpan timeout)[] args)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Timeout)}"))
            {
                _moderator.Timeout(args);
            }
        }
        public void Moderate(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Moderate)}"))
            {
                _moderator.Moderate(usernames);
            }
        }
        public void Unmoderate(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Unmoderate)}"))
            {
                _moderator.Unmoderate(usernames);
            }
        }
        public void Promote(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Promote)}"))
            {
                _moderator.Promote(usernames);
            }
        }
        public void Demote(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Demote)}"))
            {
                _moderator.Demote(usernames);
            }
        }
        public void Welcome(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Welcome)}"))
            {
                _moderator.Welcome(usernames);
            }
        }
        public void Unwelcome(params string[] usernames)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Unwelcome)}"))
            {
                _moderator.Unwelcome(usernames);
            }
        }

        public void Hold(string key, (System.Func<System.Threading.Tasks.Task> onAllowAsync, System.Func<System.Threading.Tasks.Task> onDenyAsync) callbacks)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Hold)}"))
            {
                _moderator.Hold(key, callbacks);
            }
        }
        public System.Func<System.Threading.Tasks.Task> Allow(params string[] keys)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Allow)}"))
            {
                return _moderator.Allow(keys);
            }
        }
        public System.Func<System.Threading.Tasks.Task> AllowAll()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(AllowAll)}"))
            {
                return _moderator.AllowAll();
            }
        }
        public System.Func<System.Threading.Tasks.Task> Deny(params string[] keys)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(Deny)}"))
            {
                return _moderator.Deny(keys);
            }
        }
        public System.Func<System.Threading.Tasks.Task> DenyAll()
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Moderators.StartActivity($"{ModeratorName}.{nameof(DenyAll)}"))
            {
                return _moderator.DenyAll();
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
