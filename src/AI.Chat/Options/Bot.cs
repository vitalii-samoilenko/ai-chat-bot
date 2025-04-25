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
        public string Prompt { get; set; }
        public string[] Args { get; set; }
    }
    public class Bot
    {
        public string Prompt { get; set; }
        public int Warnings { get; set; }
        public string Apology { get; set; }
        public Filter[] Filters { get; set; }
    }
}
