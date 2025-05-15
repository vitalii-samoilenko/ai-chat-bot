namespace AI.Chat.Moderators
{
    public class Slim : IModerator
    {
        private readonly Options.Moderator _options;
        private readonly System.Collections.Generic.Dictionary<string, System.DateTime> _timeouts;
        private readonly System.Collections.Generic.HashSet<System.DateTime> _onHold;
        private readonly System.Collections.Generic.HashSet<string> _greeted;

        public Slim(Options.Moderator options)
        {
            _options = options;
            _timeouts = new System.Collections.Generic.Dictionary<string, System.DateTime>();
            _onHold = new System.Collections.Generic.HashSet<System.DateTime>();
            _greeted = new System.Collections.Generic.HashSet<string>();
        }

        public bool IsModerator(string username)
        {
            return _options.Moderators.Contains(username);
        }
        public bool IsModerated(params string[] usernames)
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
        public bool IsAllowed(params string[] usernames)
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
        public bool IsWelcomed(params string[] usernames)
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

        public void Ban(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Banned.Add(username);
            }
        }
        public void Unban(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Banned.Remove(username);
            }
        }
        public void Timeout(params (string username, System.TimeSpan timeout)[] args)
        {
            var now = System.DateTime.UtcNow;
            foreach((var username, var timeout) in args)
            {
                _timeouts[username] = now + timeout;
            }
        }
        public void Moderate(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Moderated.Add(username);
            }
        }
        public void Unmoderate(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Moderated.Remove(username);
            }
        }
        public void Promote(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Promoted.Add(username);
            }
        }
        public void Demote(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Promoted.Remove(username);
            }
        }
        public void Welcome(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Welcomed.Add(username);
            }
        }
        public void Unwelcome(params string[] usernames)
        {
            foreach (var username in usernames)
            {
                _options.Welcomed.Remove(username);
            }
        }

        public void Hold(params System.DateTime[] keys)
        {
            foreach(var key in keys)
            {
                _onHold.Add(key);
            }
        }
        public void Allow(params System.DateTime[] keys)
        {
            foreach (var key in keys)
            {
                _onHold.Remove(key);
            }
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> AllowAll()
        {
            foreach (var key in _onHold)
            {
                yield return key;
            }
            _onHold.Clear();
        }
        public void Deny(params System.DateTime[] keys)
        {
            foreach (var key in keys)
            {
                _onHold.Remove(key);
            }
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> DenyAll()
        {
            foreach (var key in _onHold)
            {
                yield return key;
            }
            _onHold.Clear();
        }

        public bool Greet(string username)
        {
            return _greeted.Add(username);
        }
    }
}
