namespace TwitchLib.Client.Diagnostics
{
    public class Meters
    {
        public static System.Diagnostics.Metrics.Meter Client { get; private set; } = new System.Diagnostics.Metrics.Meter("TwitchLib.Client");
        public static System.Diagnostics.Metrics.Histogram<int> MessageLength { get; private set; } = Client.CreateHistogram<int>("twitchlib.client.message_length");
    }
}
