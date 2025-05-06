namespace AI.Chat.Adapters.Diagnostics
{
    public class ReplyLength<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private readonly IAdapter _adapter;

        public ReplyLength(TAdapter adapter)
        {
            _adapter = adapter;
        }

        public string AddInstruction(string content)
        {
            return _adapter.AddInstruction(content);
        }
        public string AddMessage(string content)
        {
            return _adapter.AddMessage(content);
        }
        public string AddReply(string content)
        {
            return _adapter.AddReply(content);
        }
        public bool TryGet(string key, out string content)
        {
            return _adapter.TryGet(key, out content);
        }
        public void Remove(string key)
        {
            _adapter.Remove(key);
        }
        public void RemoveAll()
        {
            _adapter.RemoveAll();
        }
        public System.Collections.Generic.IEnumerable<string> Find(string fromKey, string toKey)
        {
            return _adapter.Find(fromKey, toKey);
        }
        public System.Collections.Generic.IEnumerable<string> FindAll()
        {
            return _adapter.FindAll();
        }

        public async System.Threading.Tasks.Task<string> GetReplyAsync()
        {
            var reply = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            AI.Chat.Diagnostics.Meters.ReplyLength.Record(reply.Length);
            return reply;
        }
    }
}
