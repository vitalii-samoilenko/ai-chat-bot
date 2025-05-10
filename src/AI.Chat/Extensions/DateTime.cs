namespace AI.Chat.Extensions
{
    public static class DateTime
    {
        public static string ToKeyString(this System.DateTime key)
        {
            return key.ToString(Constants.HistoryKeyFormat);
        }
    }
}
