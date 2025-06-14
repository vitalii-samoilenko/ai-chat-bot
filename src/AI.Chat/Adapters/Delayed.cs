﻿namespace AI.Chat.Adapters
{
    public class Delayed<TAdapter> : IAdapter
        where TAdapter : IAdapter
    {
        private static readonly System.TimeSpan Minute = System.TimeSpan.FromMinutes(1);

        private readonly Options.Adapter _options;
        private readonly IAdapter _adapter;

        private System.DateTime _next;

        public Delayed(Options.Adapter options, TAdapter adapter)
        {
            _options = options;
            _adapter = adapter;

            _next = System.DateTime.UtcNow;
        }

        public async System.Threading.Tasks.Task<(string reply, int tokens)> GetReplyAsync()
        {
            var left = _next - System.DateTime.UtcNow;
            if (System.TimeSpan.Zero < left)
            {
                await System.Threading.Tasks.Task.Delay(left)
                    .ConfigureAwait(false);
            }
            (var reply, var tokens) = await _adapter.GetReplyAsync()
                .ConfigureAwait(false);
            var now = System.DateTime.UtcNow;
            if (!(now - _next < Minute)
                || _next.Minute < now.Minute)
            {
                _next = new System.DateTime(
                    now.Year, now.Month, now.Day,
                    now.Hour, now.Minute, 0, 0,
                    System.DateTimeKind.Utc);
            }
            _next += _options.Delay;
            return (reply, tokens);
        }
    }
}
