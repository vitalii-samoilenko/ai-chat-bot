namespace Microsoft.Extensions.Configuration
{
    public static class ConfigurationManager
    {
        public static IConfigurationManager ConfigureAIChat(this IConfigurationManager configuration)
        {
            configuration.AddJsonFile(AI.Chat.Host.Constants.JsonCommon, true);
            configuration.AddJsonFile(AI.Chat.Host.Constants.JsonModerated, true);
            switch (configuration.GetValue<string>("Chat:Client:Type"))
            {
                case nameof(AI.Chat.Clients.Twitch):
                    {
                        configuration.AddJsonFile(AI.Chat.Host.Constants.JsonOAuth, true);
                    }
                    break;
            }

            return configuration;
        }
    }
}
