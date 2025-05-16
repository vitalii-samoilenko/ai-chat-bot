namespace AI.Chat.Moderators.Host
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

        public string[] Ban(params string[] usernames)
        {
            var banned = _moderator.Ban(usernames);
            if (0 < banned.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return banned;
        }
        public string[] Unban(params string[] usernames)
        {
            var unbanned = _moderator.Unban(usernames);
            if (0 < unbanned.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return unbanned;
        }
        public (string username, System.DateTime until)[] Timeout(params (string username, System.TimeSpan timeout)[] args)
        {
            return _moderator.Timeout(args);
        }
        public string[] Moderate(params string[] usernames)
        {
            var moderated = _moderator.Moderate(usernames);
            if (0 < moderated.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return moderated;
        }
        public string[] Unmoderate(params string[] usernames)
        {
            var unmoderated = _moderator.Unmoderate(usernames);
            if (0 < unmoderated.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return unmoderated;
        }
        public string[] Promote(params string[] usernames)
        {
            var promoted = _moderator.Promote(usernames);
            if (0 < promoted.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return promoted;
        }
        public string[] Demote(params string[] usernames)
        {
            var demoted = _moderator.Demote(usernames);
            if (0 < demoted.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return demoted;
        }
        public string[] Welcome(params string[] usernames)
        {
            var welcomed = _moderator.Welcome(usernames);
            if (0 < welcomed.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return welcomed;
        }
        public string[] Unwelcome(params string[] usernames)
        {
            var unwelcomed = _moderator.Unwelcome(usernames);
            if (0 < unwelcomed.Length)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return unwelcomed;
        }

        public bool Greet(string username)
        {
            return _moderator.Greet(username);
        }
    }
}
