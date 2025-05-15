namespace AI.Chat.Scopes
{
    public class Slim : IScope
    {
        // r 0000000001 0000000000
        // r 0000000002 0000000000
        // r 0000000003 0000000000
        // w 1000000003 0000000000
        // r 1000000004 0000000000
        // r 1000000005 0000000000
        // w 2000000005 0000000000
        // w 3000000005 0000000000
        // r 3000000006 0000000000
        // r 3000000007 0000000000

        private const long ReadStep = 1L;
        private const long WriteStep = 1000000000L;

        private long _queued;
        private long _completed;
        private readonly System.Threading.ManualResetEventSlim _event;

        public Slim()
        {
            _queued = 0;
            _completed = 0;
            _event = new System.Threading.ManualResetEventSlim(true);
        }

        public void ExecuteRead(System.Action action)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, ReadStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            try
            {
                action();
            }
            finally
            { 
                System.Threading.Interlocked.Add(ref _completed, ReadStep);
            }
        }
        public T ExecuteRead<T>(System.Func<T> action)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, ReadStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            try
            {
                return action();
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, ReadStep);
            }
        }
        public System.Collections.Generic.IEnumerable<T> ExecuteRead<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, ReadStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            try
            {
                foreach (var token in action())
                {
                    yield return token;
                }
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, ReadStep);
            }
        }
        public void ExecuteWrite(System.Action action)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, WriteStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            _event.Reset();
            try
            {
                action();
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, WriteStep);
                _event.Set();
            }
        }
        public T ExecuteWrite<T>(System.Func<T> action)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, WriteStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            _event.Reset();
            try
            {
                return action();
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, WriteStep);
                _event.Set();
            }
        }
        public System.Collections.Generic.IEnumerable<T> ExecuteWrite<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, WriteStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            try
            {
                foreach (var token in action())
                {
                    yield return token;
                }
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, ReadStep);
            }
        }

        public async System.Threading.Tasks.Task ExecuteReadAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, ReadStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            try
            {
                await actionAsync()
                    .ConfigureAwait(false);
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, ReadStep);
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteReadAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, ReadStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            try
            {
                return await actionAsync()
                    .ConfigureAwait(false);
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, ReadStep);
            }
        }
        public async System.Threading.Tasks.Task ExecuteWriteAsync(System.Func<System.Threading.Tasks.Task> actionAsync)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, WriteStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            _event.Reset();
            try
            {
                await actionAsync()
                    .ConfigureAwait(false);
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, WriteStep);
                _event.Set();
            }
        }
        public async System.Threading.Tasks.Task<T> ExecuteWriteAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync)
        {
            var spinner = new System.Threading.SpinWait();
            for (var current = System.Threading.Interlocked.Add(ref _queued, WriteStep);
                WriteStep < current - System.Threading.Interlocked.Read(ref _completed);)
            {
                if (_event.IsSet)
                {
                    spinner.SpinOnce();
                }
                else
                {
                    _event.Wait();
                }
            }
            _event.Reset();
            try
            {
                return await actionAsync()
                    .ConfigureAwait(false);
            }
            finally
            {
                System.Threading.Interlocked.Add(ref _completed, WriteStep);
                _event.Set();
            }
        }
    }
}
