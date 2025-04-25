namespace AI.Chat
{
    public interface IBot
    {
        bool TryGet(string key, out string message);
        void Remove(params string[] keys);
        void RemoveAll();
        string Instruct(string message);
        System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey);
        System.Collections.Generic.IEnumerable<string> FindAll();

        System.Threading.Tasks.Task<(string messageKey, string replyKey)> ReplyAsync(string message);
    }
}
