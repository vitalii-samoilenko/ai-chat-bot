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
        public bool IsModerated(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteRead(() => _moderator.IsModerated(usernames));
        }
        public bool IsAllowed(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteRead(() => _moderator.IsAllowed(usernames));
        }
        public bool IsWelcomed(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteRead(() => _moderator.IsWelcomed(usernames));
        }

        public System.Collections.Generic.List<string> Ban(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Ban(usernames));
        }
        public System.Collections.Generic.List<string> Unban(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Unban(usernames));
        }
        public System.Collections.Generic.List<(string username, System.DateTime until)> Timeout(System.Collections.Generic.IEnumerable<(string username, System.TimeSpan timeout)> args)
        {
            return _scope.ExecuteWrite(() => _moderator.Timeout(args));
        }
        public System.Collections.Generic.List<string> Moderate(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Moderate(usernames));
        }
        public System.Collections.Generic.List<string> Unmoderate(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Unmoderate(usernames));
        }
        public System.Collections.Generic.List<string> Promote(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Promote(usernames));
        }
        public System.Collections.Generic.List<string> Demote(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Demote(usernames));
        }
        public System.Collections.Generic.List<string> Welcome(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Welcome(usernames));
        }
        public System.Collections.Generic.List<string> Unwelcome(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _scope.ExecuteWrite(() => _moderator.Unwelcome(usernames));
        }

        public bool Greet(string username)
        {
            return _scope.ExecuteWrite(() => _moderator.Greet(username));
        }
    }
}
