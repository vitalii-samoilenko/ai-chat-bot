namespace AI.Chat.Scopes.Diagnostics
{
    public class OpenClose<TScope> : IScope
        where TScope : IScope
    {
        private readonly TScope _scope;

        public OpenClose(TScope scope)
        {
            _scope = scope;
        }

        public void ExecuteRead(System.Action action)
        {
            Chat.Diagnostics.Meters.ReadOpen.Add(1);
            try
            {
                _scope.ExecuteRead(action);
            }
            finally
            {
                Chat.Diagnostics.Meters.ReadClose.Add(1);
            }
        }
        public T ExecuteRead<T>(System.Func<T> action)
        {
            Chat.Diagnostics.Meters.ReadOpen.Add(1);
            try
            {
                return _scope.ExecuteRead(action);
            }
            finally
            {
                Chat.Diagnostics.Meters.ReadClose.Add(1);
            }
        }
        public System.Collections.Generic.IEnumerable<T> ExecuteRead<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action)
        {
            Chat.Diagnostics.Meters.ReadOpen.Add(1);
            try
            {
                foreach (var token in _scope.ExecuteRead(action))
                {
                    yield return token;
                }
            }
            finally
            {
                Chat.Diagnostics.Meters.ReadClose.Add(1);
            }
        }
        public void ExecuteWrite(System.Action action)
        {
            Chat.Diagnostics.Meters.WriteOpen.Add(1);
            try
            {
                _scope.ExecuteWrite(action);
            }
            finally
            {
                Chat.Diagnostics.Meters.WriteClose.Add(1);
            }
        }
        public T ExecuteWrite<T>(System.Func<T> action)
        {
            Chat.Diagnostics.Meters.WriteOpen.Add(1);
            try
            {
                return _scope.ExecuteWrite(action);
            }
            finally
            {
                Chat.Diagnostics.Meters.WriteClose.Add(1);
            }
        }
        public System.Collections.Generic.IEnumerable<T> ExecuteWrite<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action)
        {
            Chat.Diagnostics.Meters.WriteOpen.Add(1);
            try
            {
                foreach (var token in _scope.ExecuteWrite(action))
                {
                    yield return token;
                }
            }
            finally
            {
                Chat.Diagnostics.Meters.WriteClose.Add(1);
            }
        }

        public async System.Threading.Tasks.Task ExecuteReadAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            Chat.Diagnostics.Meters.ReadOpen.Add(1);
            try
            {
                await _scope.ExecuteReadAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Meters.ReadClose.Add(1);
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteReadAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            Chat.Diagnostics.Meters.ReadOpen.Add(1);
            try
            {
                return await _scope.ExecuteReadAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Meters.ReadClose.Add(1);
            }
        }
        public async System.Threading.Tasks.Task ExecuteWriteAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            Chat.Diagnostics.Meters.WriteOpen.Add(1);
            try
            {
                await _scope.ExecuteWriteAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Meters.WriteClose.Add(1);
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteWriteAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            Chat.Diagnostics.Meters.WriteOpen.Add(1);
            try
            {
                return await _scope.ExecuteWriteAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Meters.WriteClose.Add(1);
            }
        }
    }
}
