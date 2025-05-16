namespace AI.Chat.Extensions
{
    public static class Record
    {
        public static bool IsModerated(this AI.Chat.Record record)
        {
            foreach (var tag in record.Tags)
            {
                if (Constants.TagModerated.Equals(tag, System.StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }
            return false;
        }
    }
}
