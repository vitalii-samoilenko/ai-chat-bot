namespace AI.Chat.Extensions
{
    public static class History
    {
        public static System.DateTime AddSystemMessage(this IHistory history, string message)
        {
            return history.Add(new Record
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
            return history.Add(new Record
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
            return history.Add(new Record
            {
                Message = message,
                Tags = new[]
                {
                    $"{Constants.TagType}={Constants.TypeModel}"
                }
            });
        }
    }
}
