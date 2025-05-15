namespace AI.Chat
{
    public interface ICommand
    {
        System.Collections.Generic.IEnumerable<string> Execute(string args);
    }
}
