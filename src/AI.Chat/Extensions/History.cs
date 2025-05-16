namespace AI.Chat.Extensions
{
    public static class History
    {
        public static System.DateTime AddSystemMessage(this IHistory history, string message)
        {
            return history.Add(new AI.Chat.Record
            {
                Message = message,
                Tags = new[]
                {
                    $"{Constants.TagType}={Constants.TypeSystem}"
                }
            });
        }
        public static System.DateTime AddUserMessage(this IHistory history, string username, string message)
        {
            return history.Add(new AI.Chat.Record
            {
                Message = message,
                Tags = new[]
                {
                    $"{Constants.TagType}={Constants.TypeUser}",
                    $"{Constants.TagUsername}={username}"
                } 
            });
        }
        public static System.DateTime AddModelMessage(this IHistory history, string message)
        {
            return history.Add(new AI.Chat.Record
            {
                Message = message,
                Tags = new[]
                {
                    $"{Constants.TagType}={Constants.TypeModel}"
                }
            });
        }
        public static System.DateTime[] Moderate(this IHistory history, params System.DateTime[] keys)
        {
            return history.Tag(Constants.TagModerated, keys);
        }
        public static System.DateTime[] Unmoderate(this IHistory history, params System.DateTime[] keys)
        {
            return history.Untag(Constants.TagModerated, keys);
        }
        public static System.Collections.Generic.IEnumerable<System.DateTime> FindModerated(this IHistory history, System.DateTime fromKey, System.DateTime toKey)
        {
            return history.Find(fromKey, toKey, Constants.TagModerated);
        }
    }
}
