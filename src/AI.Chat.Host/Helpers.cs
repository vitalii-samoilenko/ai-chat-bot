using AI.Chat.Extensions;

namespace AI.Chat.Host
{
    internal static class Helpers
    {
        public static void Save(AI.Chat.Options.Moderator options)
        {
            System.IO.File.WriteAllText(
                Constants.JsonModerated,
                System.Text.Json.JsonSerializer.Serialize(
                    new
                    {
                        Chat = new
                        {
                            Moderator = new
                            {
                                options.Promoted,
                                options.Banned,
                                options.Moderated,
                                options.Welcomed
                            }
                        }
                    },
                    new System.Text.Json.JsonSerializerOptions
                    {
                        WriteIndented = true
                    }));
        }
        public static void Save(AI.Chat.Options.Twitch.Client options)
        {
            System.IO.File.WriteAllText(
                Constants.JsonOAuth,
                System.Text.Json.JsonSerializer.Serialize(
                    new
                    {
                        Chat = new
                        {
                            Client = new
                            {
                                Auth = new
                                {
                                    options.Auth.AccessToken,
                                    options.Auth.RefreshToken,
                                    options.Auth.DeviceCode
                                }
                            }
                        }
                    },
                    new System.Text.Json.JsonSerializerOptions
                    {
                        WriteIndented = true
                    }));
        }

        public static void AppendLog(System.DateTime key, AI.Chat.Record record)
        {
            System.IO.File.AppendAllLines(
                Constants.LogHistory,
                new[]
                {
                    System.Text.Json.JsonSerializer.Serialize(
                        new System.Collections.Generic.KeyValuePair<string, AI.Chat.Record>(
                            key.ToKeyString(), record),
                            new System.Text.Json.JsonSerializerOptions
                            {
                                Encoder = System.Text.Encodings.Web.JavaScriptEncoder.Create(
                                    System.Text.Unicode.UnicodeRanges.All)
                            })
                });
        }
        public static void DeleteLog(params System.DateTime[] keys)
        {
            var lines = new string[keys.Length];
            for (var i = 0; i < keys.Length; ++i)
            {
                lines[i] = keys[i].ToKeyString();
            }
            System.IO.File.AppendAllLines(
                Constants.LogDeleted,
                lines);
        }
        public static void DeleteLog()
        {
            System.IO.File.Delete(Constants.LogHistory);
            System.IO.File.Delete(Constants.LogDeleted);
        }
        public static System.Collections.Generic.TimeSeries<AI.Chat.Record> LoadLog()
        {
            System.Collections.Generic.TimeSeries<AI.Chat.Record> history = null;
            var newHistoryLog = new System.Collections.Generic.List<string>();
            if (System.IO.File.Exists(Constants.LogHistory))
            {
                var deleted = new System.Collections.Generic.HashSet<string>(
                    System.IO.File.Exists(Constants.LogDeleted)
                        ? System.IO.File.ReadAllLines(Constants.LogDeleted)
                        : new string[] { },
                    System.StringComparer.OrdinalIgnoreCase);
                var currentHistoryLog = System.IO.File.ReadAllLines(Constants.LogHistory);
                var startLine = currentHistoryLog[0];
                history = new System.Collections.Generic.TimeSeries<AI.Chat.Record>(startLine.ParseKey());
                newHistoryLog.Add(startLine);
                for (var i = 1; i < currentHistoryLog.Length; ++i)
                {
                    var line = currentHistoryLog[i];
                    var pair = System.Text.Json.JsonSerializer.Deserialize<System.Collections.Generic.KeyValuePair<string, AI.Chat.Record>>(line);
                    if (deleted.Contains(pair.Key))
                    {
                        continue;
                    }
                    history.Add(pair.Key.ParseKey(), pair.Value);
                    newHistoryLog.Add(line);
                }
            }
            else
            {
                history = new System.Collections.Generic.TimeSeries<Record>();
                newHistoryLog.Add(history.Start.ToKeyString());
            }
            System.IO.File.Delete(Constants.LogDeleted);
            System.IO.File.WriteAllLines(Constants.LogHistory, newHistoryLog);
            return history;
        }
    }
}
