namespace AI.Chat.Bots
{
    public class Slim : IBot
    {
        private readonly IAdapter _adapter;

        public Slim(IAdapter adapter)
        {
            _adapter = adapter;
        }

        public bool TryGet(string key, out string message)
        {
            return _adapter.TryGet(key, out message);
        }
        public void Remove(params string[] keys)
        {
            foreach (var key in keys)
            {
                _adapter.Remove(key);
            }
        }
        public void RemoveAll()
        {
            _adapter.RemoveAll();
        }
        public string Instruct(string message)
        {
            return _adapter.AddInstruction(message);
        }
        public System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey)
        {
            return _adapter.Find(fromKey, toKey);
        }
        public System.Collections.Generic.IEnumerable<string> FindAll()
        {
            return _adapter.FindAll();
        }

        public async System.Threading.Tasks.Task<(string messageKey, string replyKey)> ReplyAsync(string message)
        {
            var messageKey = _adapter.AddMessage(message);
            var reply = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            var replyKey = _adapter.AddReply(reply);
            return (messageKey, replyKey);
        }
    }
}
