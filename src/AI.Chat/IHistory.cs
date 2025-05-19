namespace AI.Chat
{
    public struct Record
    {
        public string Message { get; set; }
        public System.Collections.Generic.List<string> Tags { get; set; }
    }
    public interface IHistory
    {
        System.DateTime Add(Record record);
        System.Collections.Generic.List<System.DateTime> Remove(System.Collections.Generic.IEnumerable<System.DateTime> keys);
        void Clear();
        System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, System.Collections.Generic.IEnumerable<string> tags);
        bool TryGet(System.DateTime key, out Record record);
        bool TryEdit(System.DateTime key, string message);
        System.Collections.Generic.List<System.DateTime> Tag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys);
        System.Collections.Generic.List<System.DateTime> Untag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys);
    }
}
