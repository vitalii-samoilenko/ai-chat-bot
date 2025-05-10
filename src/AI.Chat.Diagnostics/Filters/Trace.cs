namespace AI.Chat.Filters.Diagnostics
{
    public class Trace<TFilter> : IFilter
        where TFilter : IFilter
    {
        private static string FilterName = $"{typeof(TFilter).Namespace}.{typeof(TFilter).Name}";

        private readonly TFilter _filter;

        public Trace(TFilter filter)
        {
            _filter = filter;
        }

        public bool IsDenied(string message, out string reason)
        {
            using (var activity = AI.Chat.Diagnostics.ActivitySources.Filters.StartActivity($"{FilterName}.{nameof(IsDenied)}"))
            {
                return _filter.IsDenied(message, out reason);
            }
        }
    }
}
