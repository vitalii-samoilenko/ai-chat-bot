namespace AI.Chat.Filters
{
    public class Regex : IFilter
    {
        private readonly string _prompt;
        private readonly System.Text.RegularExpressions.Regex _regex;

        public Regex(string prompt, string pattern)
        {
            _prompt = prompt;
            _regex = new System.Text.RegularExpressions.Regex(pattern);
        }

        public string Prompt => _prompt;

        public bool IsDenied(string message)
        {
            return _regex.IsMatch(message);
        }
    }
}
