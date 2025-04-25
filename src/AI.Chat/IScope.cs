namespace AI.Chat
{
    public interface IScope
    {
        void ExecuteRead(System.Action action);
        T ExecuteRead<T>(System.Func<T> action);
        System.Collections.Generic.IEnumerable<T> ExecuteRead<T>(System.Func<System.Collections.Generic.IEnumerable<T>> action);
        void ExecuteWrite(System.Action action);
        T ExecuteWrite<T>(System.Func<T> action);

        System.Threading.Tasks.Task ExecuteReadAsync(System.Func<System.Threading.Tasks.Task> actionAsync);
        System.Threading.Tasks.Task<T> ExecuteReadAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync);
        System.Threading.Tasks.Task ExecuteWriteAsync(System.Func<System.Threading.Tasks.Task> actionAsync);
        System.Threading.Tasks.Task<T> ExecuteWriteAsync<T>(System.Func<System.Threading.Tasks.Task<T>> actionAsync);
    }
}
