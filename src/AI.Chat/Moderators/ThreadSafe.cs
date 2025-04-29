namespace AI.Chat.Moderators
{
    public class ThreadSafe<TModerator> : IModerator
        where TModerator : IModerator
    {
        private readonly IModerator _moderator;
        private readonly IScope _scope;

        public ThreadSafe(TModerator moderator, IScope scope)
        {
            _moderator = moderator;
            _scope = scope;
        }

        public bool IsModerator(string username)
        {
            return _scope.ExecuteRead(() => _moderator.IsModerator(username));
        }
        public bool IsModerated(params string[] usernames)
        {
            return _scope.ExecuteRead(() => _moderator.IsModerated(usernames));
        }
        public bool IsAllowed(params string[] usernames)
        {
            return _scope.ExecuteRead(() => _moderator.IsAllowed(usernames));
        }
        public bool IsWelcomed(params string[] usernames)
        {
            return _scope.ExecuteRead(() => _moderator.IsWelcomed(usernames));
        }

        public void Ban(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Ban(usernames));
        }
        public void Unban(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Unban(usernames));
        }
        public void Timeout(params (string username, System.TimeSpan timeout)[] args)
        {
            _scope.ExecuteWrite(() => _moderator.Timeout(args));
        }
        public void Moderate(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Moderate(usernames));
        }
        public void Unmoderate(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Unmoderate(usernames));
        }
        public void Promote(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Unmoderate(usernames));
        }
        public void Demote(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Demote(usernames));
        }
        public void Welcome(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Welcome(usernames));
        }
        public void Unwelcome(params string[] usernames)
        {
            _scope.ExecuteWrite(() => _moderator.Unwelcome(usernames));
        }

        public void Hold(string key, (System.Func<System.Threading.Tasks.Task> onAllowAsync, System.Func<System.Threading.Tasks.Task> onDenyAsync) callbacks)
        {
            _scope.ExecuteWrite(() => _moderator.Hold(key, callbacks));
        }
        public System.Func<System.Threading.Tasks.Task> Allow(params string[] keys)
        {
            return _scope.ExecuteWrite(() => _moderator.Allow(keys));
        }
        public System.Func<System.Threading.Tasks.Task> AllowAll()
        {
            return _scope.ExecuteWrite(() => _moderator.AllowAll());
        }
        public System.Func<System.Threading.Tasks.Task> Deny(params string[] keys)
        {
            return _scope.ExecuteWrite(() => _moderator.Deny(keys));
        }
        public System.Func<System.Threading.Tasks.Task> DenyAll()
        {
            return _scope.ExecuteWrite(() => _moderator.DenyAll());
        }
    }
}
