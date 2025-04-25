namespace AI.Chat
{
    public interface ICommand
    {
        System.Threading.Tasks.Task ExecuteAsync(string args);
    }
}
