namespace AI.Chat.Extensions
{
    public static class String
    {
        public static System.DateTime ParseKey(this string value)
        {
            return System.DateTime.ParseExact(value, Constants.HistoryKeyFormat, null,
                System.Globalization.DateTimeStyles.AssumeUniversal | System.Globalization.DateTimeStyles.AdjustToUniversal);
        }
        public static bool TryParseKey(this string value, out System.DateTime result)
        {
            return System.DateTime.TryParseExact(value, Constants.HistoryKeyFormat, null,
                System.Globalization.DateTimeStyles.AssumeUniversal | System.Globalization.DateTimeStyles.AdjustToUniversal,
                out result);
        }
        public static string[] SplitArgs(this string args)
        {
            return args.Split(new[] { ' ' }, System.StringSplitOptions.RemoveEmptyEntries);
        }
        public static string ExtractToken(this string value, out string remainder)
        {
            var previous = 0;
            var next = value.IndexOf(' ', previous);
            if (next < 0)
            {
                remainder = string.Empty;
                next = value.Length;
            }
            else
            {
                remainder = value.Substring(next + 1);
            }
            return value.Substring(previous, next - previous);
        }
    }
}
