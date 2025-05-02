namespace AI.Chat.Moderators
{
    public class Slim : IModerator
    {
        private readonly Options.Moderator _options;
        private readonly System.Collections.Generic.Dictionary<string, System.DateTime> _timeouts;
        private readonly System.Collections.Generic.Dictionary<string, (System.Func<System.Threading.Tasks.Task>, System.Func<System.Threading.Tasks.Task>)> _onHold;
        private readonly System.Collections.Generic.HashSet<string> _greeted;

        public Slim(Options.Moderator options)
        {
            _options = options;
            _timeouts = new System.Collections.Generic.Dictionary<string, System.DateTime>();
            _onHold = new System.Collections.Generic.Dictionary<string, (System.Func<System.Threading.Tasks.Task>, System.Func<System.Threading.Tasks.Task>)>();
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
                if (!_options.Welcomed.Contains(username))
                {
                    continue;
                }
                if (_greeted.Contains(username))
                {
                    return false;
                }
                welcomed = true;
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

        public void Hold(string key, (System.Func<System.Threading.Tasks.Task> onAllowAsync, System.Func<System.Threading.Tasks.Task> onDenyAsync) callbacks)
        {
            _onHold[key] = callbacks;
        }
        public System.Func<System.Threading.Tasks.Task> Allow(params string[] keys)
        {
            var callbacks = new System.Collections.Generic.List<System.Func<System.Threading.Tasks.Task>>();
            foreach (var key in keys)
            {
                if (_onHold.TryGetValue(key, out var pair))
                {
                    (var onAllowAsync, _) = pair;
                    _onHold.Remove(key);
                    callbacks.Add(onAllowAsync);
                }
            }
            return async () =>
            {
                foreach (var onAllowAsync in callbacks)
                {
                    await onAllowAsync()
                        .ConfigureAwait(false);
                }
            };
        }
        public System.Func<System.Threading.Tasks.Task> AllowAll()
        {
            var callbacks = new System.Collections.Generic.List<System.Func<System.Threading.Tasks.Task>>();
            foreach (var pair in _onHold.Values)
            {
                (var onAllowAsync, _) = pair;
                callbacks.Add(onAllowAsync);
            }
            _onHold.Clear();
            return async () =>
            {
                foreach (var onAllowAsync in callbacks)
                {
                    await onAllowAsync()
                        .ConfigureAwait(false);
                }
            };
        }
        public System.Func<System.Threading.Tasks.Task> Deny(params string[] keys)
        {
            var callbacks = new System.Collections.Generic.List<System.Func<System.Threading.Tasks.Task>>();
            foreach (var key in keys)
            {
                if (_onHold.TryGetValue(key, out var pair))
                {
                    (_, var onDenyAsync) = pair;
                    _onHold.Remove(key);
                    callbacks.Add(onDenyAsync);
                }
            }
            return async () =>
            {
                foreach (var onDenyAsync in callbacks)
                {
                    await onDenyAsync()
                        .ConfigureAwait(false);
                }
            };
        }
        public System.Func<System.Threading.Tasks.Task> DenyAll()
        {
            var callbacks = new System.Collections.Generic.List<System.Func<System.Threading.Tasks.Task>>();
            foreach (var pair in _onHold.Values)
            {
                (_, var onDenyAsync) = pair;
                callbacks.Add(onDenyAsync);
            }
            _onHold.Clear();
            return async () =>
            {
                foreach (var onDenyAsync in callbacks)
                {
                    await onDenyAsync()
                        .ConfigureAwait(false);
                }
            };
        }

        public bool Greet(string username)
        {
            return _greeted.Add(username);
        }
    }
}
