namespace AI.Chat
{
    public interface IBot
    {
        System.Threading.Tasks.Task<(System.DateTime messageKey, System.DateTime replyKey)> ReplyAsync(string username, string message);
    }
}
