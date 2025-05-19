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
        public bool IsModerated(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _moderator.IsModerated(usernames);
        }
        public bool IsAllowed(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _moderator.IsAllowed(usernames);
        }
        public bool IsWelcomed(System.Collections.Generic.IEnumerable<string> usernames)
        {
            return _moderator.IsWelcomed(usernames);
        }

        public System.Collections.Generic.List<string> Ban(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var banned = _moderator.Ban(usernames);
            if (0 < banned.Count)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return banned;
        }
        public System.Collections.Generic.List<string> Unban(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var unbanned = _moderator.Unban(usernames);
            if (0 < unbanned.Count)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return unbanned;
        }
        public System.Collections.Generic.List<(string username, System.DateTime until)> Timeout(System.Collections.Generic.IEnumerable<(string username, System.TimeSpan timeout)> args)
        {
            return _moderator.Timeout(args);
        }
        public System.Collections.Generic.List<string> Moderate(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var moderated = _moderator.Moderate(usernames);
            if (0 < moderated.Count)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return moderated;
        }
        public System.Collections.Generic.List<string> Unmoderate(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var unmoderated = _moderator.Unmoderate(usernames);
            if (0 < unmoderated.Count)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return unmoderated;
        }
        public System.Collections.Generic.List<string> Promote(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var promoted = _moderator.Promote(usernames);
            if (0 < promoted.Count)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return promoted;
        }
        public System.Collections.Generic.List<string> Demote(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var demoted = _moderator.Demote(usernames);
            if (0 < demoted.Count)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return demoted;
        }
        public System.Collections.Generic.List<string> Welcome(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var welcomed = _moderator.Welcome(usernames);
            if (0 < welcomed.Count)
            {
                AI.Chat.Host.Helpers.Save(_options);
            }
            return welcomed;
        }
        public System.Collections.Generic.List<string> Unwelcome(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var unwelcomed = _moderator.Unwelcome(usernames);
            if (0 < unwelcomed.Count)
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
