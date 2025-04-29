namespace AI.Chat.Moderators.Console
{
    internal class Persistent<TModerator> : IModerator
        where TModerator : IModerator
    {
        private readonly Options.Moderator _options;
        private readonly IModerator _moderator;

        public Persistent(Options.Moderator options, TModerator moderator)
        {
            _options = options;
            _moderator = moderator;
        }

        public bool IsModerator(string username)
        {
            return _moderator.IsModerator(username);
        }
        public bool IsModerated(params string[] usernames)
        {
            return _moderator.IsModerated(usernames);
        }
        public bool IsAllowed(params string[] usernames)
        {
            return _moderator.IsAllowed(usernames);
        }
        public bool IsWelcomed(params string[] usernames)
        {
            return _moderator.IsWelcomed(usernames);
        }

        public void Ban(params string[] usernames)
        {
            _moderator.Ban(usernames);
            Host.Console.Helpers.Save(_options);
        }
        public void Unban(params string[] usernames)
        {
            _moderator.Unban(usernames);
            Host.Console.Helpers.Save(_options);
        }
        public void Timeout(params (string username, System.TimeSpan timeout)[] args)
        {
            _moderator.Timeout(args);
        }
        public void Moderate(params string[] usernames)
        {
            _moderator.Moderate(usernames);
            Host.Console.Helpers.Save(_options);
        }
        public void Unmoderate(params string[] usernames)
        {
            _moderator.Unmoderate(usernames);
            Host.Console.Helpers.Save(_options);
        }
        public void Promote(params string[] usernames)
        {
            _moderator.Unmoderate(usernames);
            Host.Console.Helpers.Save(_options);
        }
        public void Demote(params string[] usernames)
        {
            _moderator.Demote(usernames);
            Host.Console.Helpers.Save(_options);
        }
        public void Welcome(params string[] usernames)
        {
            _moderator.Welcome(usernames);
            Host.Console.Helpers.Save(_options);
        }
        public void Unwelcome(params string[] usernames)
        {
            _moderator.Unwelcome(usernames);
            Host.Console.Helpers.Save(_options);
        }

        public void Hold(string key, (System.Func<System.Threading.Tasks.Task> onAllowAsync, System.Func<System.Threading.Tasks.Task> onDenyAsync) callbacks)
        {
            _moderator.Hold(key, callbacks);
        }
        public System.Func<System.Threading.Tasks.Task> Allow(params string[] keys)
        {
            return _moderator.Allow(keys);
        }
        public System.Func<System.Threading.Tasks.Task> AllowAll()
        {
            return _moderator.AllowAll();
        }
        public System.Func<System.Threading.Tasks.Task> Deny(params string[] keys)
        {
            return _moderator.Deny(keys);
        }
        public System.Func<System.Threading.Tasks.Task> DenyAll()
        {
            return _moderator.DenyAll();
        }
    }
}
