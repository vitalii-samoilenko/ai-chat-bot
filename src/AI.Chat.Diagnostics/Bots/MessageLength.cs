namespace AI.Chat.Bots.Diagnostics
{
    public class MessageLength<TBot> : IBot
        where TBot : IBot
    {
        private readonly IBot _bot;

        public MessageLength(TBot bot)
        {
            _bot = bot;
        }

        public async System.Threading.Tasks.Task<(System.DateTime messageKey, System.DateTime replyKey)> ReplyAsync(string username, string message)
        {
            AI.Chat.Diagnostics.Meters.MessageLength.Record(message.Length,
                new System.Collections.Generic.KeyValuePair<string, object>("user.name", username));
            return await _bot.ReplyAsync(username, message)
                .ConfigureAwait(false);
        }
    }
}
