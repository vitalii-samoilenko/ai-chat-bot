namespace AI.Chat.Moderators
{
    public class Slim : IModerator
    {
        private readonly Options.Moderator _options;
        private readonly System.Collections.Generic.Dictionary<string, System.DateTime> _timeouts;
        private readonly System.Collections.Generic.HashSet<string> _greeted;

        public Slim(Options.Moderator options)
        {
            _options = options;
            _timeouts = new System.Collections.Generic.Dictionary<string, System.DateTime>(System.StringComparer.OrdinalIgnoreCase);
            _greeted = new System.Collections.Generic.HashSet<string>(System.StringComparer.OrdinalIgnoreCase);
        }

        public bool IsModerator(string username)
        {
            return _options.Moderators.Contains(username);
        }
        public bool IsModerated(System.Collections.Generic.IEnumerable<string> usernames)
        {
            foreach (var username in usernames)
            {
                if (_options.Moderated.Contains(username))
                {
                    return true;
                }
            }
            return false;
        }
        public bool IsAllowed(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var now = System.DateTime.UtcNow;
            var promoted = false;
            foreach (var username in usernames)
            {
                if (_options.Banned.Contains(username)
                    || (_timeouts.TryGetValue(username, out var until)
                        && now < until))
                {
                    return false;
                }
                promoted |= _options.Promoted.Contains(username);
            }
            return promoted;
        }
        public bool IsWelcomed(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var welcomed = false;
            foreach (var username in usernames)
            {
                if (_greeted.Contains(username))
                {
                    return false;
                }
                welcomed |= _options.Welcomed.Contains(username);
            }
            return welcomed;
        }

        public System.Collections.Generic.List<string> Ban(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var banned = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Banned.Add(username))
                {
                    banned.Add(username);
                }
            }
            return banned;
        }
        public System.Collections.Generic.List<string> Unban(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var unbanned = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Banned.Remove(username))
                {
                    unbanned.Add(username);
                }
            }
            return unbanned;
        }
        public System.Collections.Generic.List<(string username, System.DateTime until)> Timeout(System.Collections.Generic.IEnumerable<(string username, System.TimeSpan timeout)> args)
        {
            var now = System.DateTime.UtcNow;
            var timeouted = new System.Collections.Generic.List<(string, System.DateTime)>();
            foreach((var username, var timeout) in args)
            {
                var until = now + timeout;
                _timeouts[username] = until;
                timeouted.Add((username, until));
            }
            return timeouted;
        }
        public System.Collections.Generic.List<string> Moderate(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var moderated = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Moderated.Add(username))
                {
                    moderated.Add(username);
                }
            }
            return moderated;
        }
        public System.Collections.Generic.List<string> Unmoderate(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var unmoderated = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Moderated.Remove(username))
                {
                    unmoderated.Add(username);
                }
            }
            return unmoderated;
        }
        public System.Collections.Generic.List<string> Promote(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var promoted = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Promoted.Add(username))
                {
                    promoted.Add(username);
                }
            }
            return promoted;
        }
        public System.Collections.Generic.List<string> Demote(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var demoted = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Promoted.Remove(username))
                {
                    demoted.Add(username);
                }
            }
            return demoted;
        }
        public System.Collections.Generic.List<string> Welcome(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var welcomed = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Welcomed.Add(username))
                {
                    welcomed.Add(username);
                }
            }
            return welcomed;
        }
        public System.Collections.Generic.List<string> Unwelcome(System.Collections.Generic.IEnumerable<string> usernames)
        {
            var unwelcomed = new System.Collections.Generic.List<string>();
            foreach (var username in usernames)
            {
                if (_options.Welcomed.Remove(username))
                {
                    unwelcomed.Add(username);
                }
            }
            return unwelcomed;
        }

        public bool Greet(string username)
        {
            return _greeted.Add(username);
        }
    }
}
