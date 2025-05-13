namespace AI.Chat
{
    public struct Record
    {
        public string Message { get; set; }
        public string[] Tags { get; set; }
    }
    public interface IHistory
    {
        System.DateTime Add(Record record);
        void Remove(params System.DateTime[] keys);
        void Clear();
        System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, params string[] tags);
        bool TryGet(System.DateTime key, out Record record);
        bool TryEdit(System.DateTime key, string message);
    }
}
