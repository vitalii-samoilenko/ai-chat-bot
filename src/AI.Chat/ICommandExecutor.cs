namespace AI.Chat
{
    public interface ICommandExecutor
    {
        System.Collections.Generic.IEnumerable<string> Execute(string username, string command, string args);
    }
}
