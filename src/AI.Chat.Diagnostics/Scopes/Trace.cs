namespace AI.Chat.Scopes.Diagnostics
{
    public class Trace<TScope> : IScope
        where TScope : IScope
    {
        private static string ScopeName = $"{typeof(TScope).Namespace}.{typeof(TScope).Name}";

        private readonly TScope _scope;

        public Trace(TScope scope)
        {
            _scope = scope;
        }

        public void ExecuteRead(System.Action action)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteRead)}"))
            {
                _scope.ExecuteRead(action);
            }
        }
        public T ExecuteRead<T>(System.Func<T> action)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteRead)}"))
            {
                return _scope.ExecuteRead(action);
            }
        }
        public System.Collections.Generic.IEnumerable<T> ExecuteRead<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action)
        {
            var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteRead)}");
            System.Collections.Generic.IEnumerator<T> enumerator = null;
            try
            {
                enumerator = _scope.ExecuteRead(action).GetEnumerator();
                while (enumerator.MoveNext())
                {
                    yield return enumerator.Current;
                }
            }
            finally
            {
                if (activity != null)
                {
                    activity.Dispose();
                }
                if (enumerator != null)
                {
                    enumerator.Dispose();
                }
            }
        }
        public void ExecuteWrite(System.Action action)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteWrite)}"))
            {
                _scope.ExecuteWrite(action);
            }
        }
        public T ExecuteWrite<T>(System.Func<T> action)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteWrite)}"))
            {
                return _scope.ExecuteWrite(action);
            }
        }
        public System.Collections.Generic.IEnumerable<T> ExecuteWrite<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action)
        {
            var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteWrite)}");
            System.Collections.Generic.IEnumerator<T> enumerator = null;
            try
            {
                enumerator = _scope.ExecuteWrite(action).GetEnumerator();
                while (enumerator.MoveNext())
                {
                    yield return enumerator.Current;
                }
            }
            finally
            {
                if (activity != null)
                {
                    activity.Dispose();
                }
                if (enumerator != null)
                {
                    enumerator.Dispose();
                }
            }
        }

        public async System.Threading.Tasks.Task ExecuteReadAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteReadAsync)}"))
            {
                await _scope.ExecuteReadAsync(actionAsync)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteReadAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteReadAsync)}"))
            {
                return await _scope.ExecuteReadAsync(actionAsync)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task ExecuteWriteAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteWriteAsync)}"))
            {
                await _scope.ExecuteWriteAsync(actionAsync)
                    .ConfigureAwait(false);
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteWriteAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Scopes.StartActivity($"{ScopeName}.{nameof(ExecuteWriteAsync)}"))
            {
                return await _scope.ExecuteWriteAsync(actionAsync)
                    .ConfigureAwait(false);
            }
        }
    }
}
