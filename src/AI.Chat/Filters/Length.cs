namespace AI.Chat.Filters
{
    public class Length : IFilter
    {
        private readonly string _prompt;
        private readonly int _length;

        public Length(string prompt, int length)
        {
            _prompt = prompt;
            _length = length;
        }

        public string Prompt => _prompt;

        public bool IsDenied(string message)
        {
            return _length < message.Length;
        }
    }
}
