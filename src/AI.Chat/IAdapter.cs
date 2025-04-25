namespace AI.Chat
{
    public interface IAdapter
    {
        string AddInstruction(string content);
        string AddMessage(string content);
        string AddReply(string content);
        bool TryGet(string key, out string content);
        void Remove(string key);
        void RemoveAll();
        System.Collections.Generic.IEnumerable<string> Find(string from, string to);
        System.Collections.Generic.IEnumerable<string> FindAll();

        System.Threading.Tasks.Task<string> GetReplyAsync();
    }
}
