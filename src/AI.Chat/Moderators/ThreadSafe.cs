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

        public string[] Ban(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Ban(usernames));
        }
        public string[] Unban(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Unban(usernames));
        }
        public (string username, System.DateTime until)[] Timeout(params (string username, System.TimeSpan timeout)[] args)
        {
            return _scope.ExecuteWrite(() => _moderator.Timeout(args));
        }
        public string[] Moderate(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Moderate(usernames));
        }
        public string[] Unmoderate(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Unmoderate(usernames));
        }
        public string[] Promote(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Promote(usernames));
        }
        public string[] Demote(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Demote(usernames));
        }
        public string[] Welcome(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Welcome(usernames));
        }
        public string[] Unwelcome(params string[] usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Unwelcome(usernames));
        }

        public bool Greet(string username)
        {
            return _scope.ExecuteWrite(() => _moderator.Greet(username));
        }
    }
}
