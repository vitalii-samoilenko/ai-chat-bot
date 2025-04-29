namespace AI.Chat.Host.Console
{
    internal static class Helpers
    {
        public static void Save(AI.Chat.Options.Moderator options)
        {
            System.IO.File.WriteAllText(
                "moderated.json",
                System.Text.Json.JsonSerializer.Serialize(
                    new
                    {
                        Chat = new
                        {
                            Moderator = new
                            {
                                Promoted = options.Promoted,
                                Banned = options.Banned,
                                Moderated = options.Moderated,
                                Welcomed = options.Welcomed
                            }
                        }
                    },
                    new System.Text.Json.JsonSerializerOptions
                    {
                        WriteIndented = true,
                        Encoder = System.Text.Encodings.Web.JavaScriptEncoder.Create(
                            System.Text.Unicode.UnicodeRanges.All)
                    }));
        }

        public static void Save(AI.Chat.Options.Twitch.Client options)
        {
            System.IO.File.WriteAllText(
                "oauth.json",
                System.Text.Json.JsonSerializer.Serialize(
                    new
                    {
                        Chat = new
                        {
                            Client = new
                            {
                                Auth = new
                                {
                                    AccessToken = options.Auth.AccessToken,
                                    RefreshToken = options.Auth.RefreshToken,
                                    DeviceCode = options.Auth.DeviceCode
                                }
                            }
                        }
                    },
                    new System.Text.Json.JsonSerializerOptions
                    {
                        WriteIndented = true
                    }));
        }
    }
}
