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
            Chat.Diagnostics.Counters.ReadOpen.Add(1);
            try
            {
                _scope.ExecuteRead(action);
            }
            finally
            {
                Chat.Diagnostics.Counters.ReadClose.Add(1);
            }
        }
        public T ExecuteRead<T>(System.Func<T> action)
        {
            Chat.Diagnostics.Counters.ReadOpen.Add(1);
            try
            {
                return _scope.ExecuteRead(action);
            }
            finally
            {
                Chat.Diagnostics.Counters.ReadClose.Add(1);
            }
        }
        public System.Collections.Generic.IEnumerable<T> ExecuteRead<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action)
        {
            Chat.Diagnostics.Counters.ReadOpen.Add(1);
            try
            {
                foreach (var result in _scope.ExecuteRead(action))
                {
                    yield return result;
                }
            }
            finally
            {
                Chat.Diagnostics.Counters.ReadClose.Add(1);
            }
        }
        public void ExecuteWrite(System.Action action)
        {
            Chat.Diagnostics.Counters.WriteOpen.Add(1);
            try
            {
                _scope.ExecuteWrite(action);
            }
            finally
            {
                Chat.Diagnostics.Counters.WriteClose.Add(1);
            }
        }
        public T ExecuteWrite<T>(System.Func<T> action)
        {
            Chat.Diagnostics.Counters.WriteOpen.Add(1);
            try
            {
                return _scope.ExecuteWrite(action);
            }
            finally
            {
                Chat.Diagnostics.Counters.WriteClose.Add(1);
            }
        }

        public async System.Threading.Tasks.Task ExecuteReadAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            Chat.Diagnostics.Counters.ReadOpen.Add(1);
            try
            {
                await _scope.ExecuteReadAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Counters.ReadClose.Add(1);
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteReadAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            Chat.Diagnostics.Counters.ReadOpen.Add(1);
            try
            {
                return await _scope.ExecuteReadAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Counters.ReadClose.Add(1);
            }
        }
        public async System.Threading.Tasks.Task ExecuteWriteAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            Chat.Diagnostics.Counters.WriteOpen.Add(1);
            try
            {
                await _scope.ExecuteWriteAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Counters.WriteClose.Add(1);
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteWriteAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            Chat.Diagnostics.Counters.WriteOpen.Add(1);
            try
            {
                return await _scope.ExecuteWriteAsync(actionAsync)
                    .ConfigureAwait(false);
            }
            finally
            {
                Chat.Diagnostics.Counters.WriteClose.Add(1);
            }
        }
    }
}
