namespace AI.Chat.Filters
{
    public class Composite : IFilter
    {
        private readonly System.Collections.Generic.IEnumerable<IFilter> _filters;

        public Composite(System.Collections.Generic.IEnumerable<IFilter> filters)
        {
            _filters = filters;
        }

        public bool IsDenied(string message, out string reason)
        {
            reason = default;
            foreach (var filter in _filters)
            {
                if (filter.IsDenied(message, out reason))
                {
                    return true;
                }
            }
            return false;
        }
    }
}
