namespace AI.Chat.Diagnostics
{
    public static class Meters
    {
        public static System.Diagnostics.Metrics.Meter Adapters { get; private set; } = new System.Diagnostics.Metrics.Meter("AI.Chat.Adapters");
        public static System.Diagnostics.Metrics.Histogram<int> ReplyLength { get; private set; } = Adapters.CreateHistogram<int>("ai.chat.adapters.reply_length");
        public static System.Diagnostics.Metrics.Gauge<int> Tokens { get; private set; } = Adapters.CreateGauge<int>("ai.chat.adapters.tokens");

        public static System.Diagnostics.Metrics.Meter Bots { get; private set; } = new System.Diagnostics.Metrics.Meter("AI.Chat.Bots");
        public static System.Diagnostics.Metrics.Histogram<int> MessageLength { get; private set; } = Bots.CreateHistogram<int>("ai.chat.bots.message_length");

        public static System.Diagnostics.Metrics.Meter Scopes { get; private set; } = new System.Diagnostics.Metrics.Meter("AI.Chat.Scopes");
        public static System.Diagnostics.Metrics.Counter<int> ReadOpen { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.read_open");
        public static System.Diagnostics.Metrics.Counter<int> ReadClose { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.read_close");
        public static System.Diagnostics.Metrics.Counter<int> WriteOpen { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.write_open");
        public static System.Diagnostics.Metrics.Counter<int> WriteClose { get; private set; } = Scopes.CreateCounter<int>("ai.chat.scopes.write_close");
    }
}
