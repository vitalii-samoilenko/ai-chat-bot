namespace AI.Chat
{
    public interface IAdapter
    {
        System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync();
    }
}
