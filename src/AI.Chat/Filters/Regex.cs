namespace AI.Chat.Filters
{
    public class Regex : IFilter
    {
        private readonly string _reason;
        private readonly System.Text.RegularExpressions.Regex _regex;

        public Regex(string reason, string pattern)
        {
            _reason = reason;
            _regex = new System.Text.RegularExpressions.Regex(pattern);
        }

        public bool IsDenied(string message, out string reason)
        {
            reason = _reason;
            return _regex.IsMatch(message);
        }
    }
}
