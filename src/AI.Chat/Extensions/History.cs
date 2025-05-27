namespace AI.Chat.Extensions
{
    public static class History
    {
        public static System.DateTime AddSystemMessage(this IHistory history, string message)
        {
            return history.Add(new AI.Chat.Record
            {
                Message = message,
                Tags = new System.Collections.Generic.List<string>
                {
                    Defaults.TagType + "=" + Defaults.TypeSystem
                }
            });
        }
        public static System.DateTime AddUserMessage(this IHistory history, string username, string message)
        {
            return history.Add(new AI.Chat.Record
            {
                Message = message,
                Tags = new System.Collections.Generic.List<string>
                {
                    Defaults.TagType + "=" + Defaults.TypeUser,
                    $"{Defaults.TagUsername}={username}"
                } 
            });
        }
        public static System.DateTime AddModelMessage(this IHistory history, string message)
        {
            return history.Add(new AI.Chat.Record
            {
                Message = message,
                Tags = new System.Collections.Generic.List<string>
                {
                    Defaults.TagType + "=" + Defaults.TypeModel
                }
            });
        }
        public static System.Collections.Generic.IEnumerable<System.DateTime> Find(this IHistory history, System.DateTime fromKey, System.DateTime toKey, params string[] tags)
        {
            return history.Find(fromKey, toKey, tags);
        }
        public static System.Collections.Generic.IEnumerable<System.DateTime> FindModerated(this IHistory history, System.DateTime fromKey, System.DateTime toKey)
        {
            return history.Find(fromKey, toKey, Defaults.TagModerated);
        }
        public static System.Collections.Generic.List<System.DateTime> Remove(this IHistory history, params System.DateTime[] keys)
        {
            return history.Remove(keys);
        }
        public static System.Collections.Generic.List<System.DateTime> Moderate(this IHistory history, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            return history.Tag(Defaults.TagModerated, keys);
        }
        public static System.Collections.Generic.List<System.DateTime> Moderate(this IHistory history, params System.DateTime[] keys)
        {
            return history.Moderate(keys);
        }
        public static System.Collections.Generic.List<System.DateTime> Unmoderate(this IHistory history, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            return history.Untag(Defaults.TagModerated, keys);
        }
    }
}
