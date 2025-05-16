namespace AI.Chat
{
    public interface IClient
    {
        System.Threading.Tasks.Task WelcomeAsync(string username, System.Func<System.DateTime, System.Threading.Tasks.Task> onAllowAsync, System.Func<System.DateTime, System.Threading.Tasks.Task> onHoldAsync);
        System.Threading.Tasks.Task ChatAsync(string username, string message, System.Func<System.DateTime, System.Threading.Tasks.Task> onAllowAsync, System.Func<System.DateTime, System.Threading.Tasks.Task> onHoldAsync);
    }
}
