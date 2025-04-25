namespace AI.Chat
{
    public interface ICommandExecutor
    {
        System.Threading.Tasks.Task<bool> ExecuteAsync(string command, string args);
    }
}
