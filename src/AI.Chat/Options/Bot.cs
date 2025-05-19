namespace AI.Chat.Options
{
    public enum FilterType
    {
        Length,
        Regex
    }
    public class Filter
    {
        public FilterType Type { get; set; }
        public string Reason { get; set; }
        public System.Collections.Generic.List<string> Args { get; set; }
    }
    public class Bot
    {
        public int Warnings { get; set; }
        public string Apology { get; set; }
        public System.Collections.Generic.List<Filter> Filters { get; set; }
    }
}
