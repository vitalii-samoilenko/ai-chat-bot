namespace AI.Chat.Filters
{
    public class Length : IFilter
    {
        private readonly string _reason;
        private readonly int _length;

        public Length(string reason, int length)
        {
            _reason = reason;
            _length = length;
        }

        public bool IsDenied(string message, out string reason)
        {
            reason = _reason;
            return _length < message.Length;
        }
    }
}
