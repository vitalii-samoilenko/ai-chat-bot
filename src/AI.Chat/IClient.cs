namespace AI.Chat
{
    public interface IClient
    {
        System.Threading.Tasks.Task WelcomeAsync(string username, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync);
        System.Threading.Tasks.Task ChatAsync(string username, string message, System.Func<string, System.Threading.Tasks.Task> onAllowAsync, System.Func<string, System.Threading.Tasks.Task> onHoldAsync);
    }
}
