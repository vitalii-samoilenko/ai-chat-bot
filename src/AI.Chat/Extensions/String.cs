namespace AI.Chat.Extensions
{
    public static class String
    {
        public static System.DateTime ParseKey(this string value)
        {
            return System.DateTime.ParseExact(value, Constants.HistoryKeyFormat, null,
                System.Globalization.DateTimeStyles.AssumeUniversal | System.Globalization.DateTimeStyles.AdjustToUniversal);
        }
        public static bool TryParseKey(this string value, out System.DateTime key)
        {
            return System.DateTime.TryParseExact(value, Constants.HistoryKeyFormat, null,
                System.Globalization.DateTimeStyles.AssumeUniversal | System.Globalization.DateTimeStyles.AdjustToUniversal,
                out key);
        }
        public static string[] SplitArgs(this string args)
        {
            return args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
        }
    }
}
