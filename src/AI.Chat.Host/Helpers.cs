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
            System.IO.File.WriteAllText(Constants.LogHistory, string.Empty);
            System.IO.File.WriteAllText(Constants.LogDeleted, string.Empty);
            System.IO.File.WriteAllText(Constants.LogEdited, string.Empty);
        }
        public static void EditLog(System.DateTime key, string message)
        {
            System.IO.File.AppendAllLines(
                Constants.LogEdited,
                new[]
                {
                    System.Text.Json.JsonSerializer.Serialize(
                        new System.Collections.Generic.KeyValuePair<string, string>(
                            key.ToKeyString(), message),
                        new System.Text.Json.JsonSerializerOptions
                        {
                            Encoder = System.Text.Encodings.Web.JavaScriptEncoder.Create(
                                System.Text.Unicode.UnicodeRanges.All)
                        })
                });
        }
        public static System.Collections.Generic.TimeSeries<AI.Chat.Record> LoadLog()
        {
            System.Collections.Generic.TimeSeries<AI.Chat.Record> history = null;
            var newHistoryLog = new System.Collections.Generic.List<string>();
            var fresh = true;
            if (System.IO.File.Exists(Constants.LogHistory))
            {
                var deleted = new System.Collections.Generic.HashSet<string>(
                    System.IO.File.Exists(Constants.LogDeleted)
                        ? System.IO.File.ReadAllLines(Constants.LogDeleted)
                        : new string[] { },
                    System.StringComparer.OrdinalIgnoreCase);
                var edited = new System.Collections.Generic.Dictionary<string, string>();
                if (System.IO.File.Exists(Constants.LogEdited))
                {
                    var editedLog = System.IO.File.ReadAllLines(Constants.LogEdited);
                    foreach (var line in editedLog)
                    {
                        var pair = System.Text.Json.JsonSerializer.Deserialize<System.Collections.Generic.KeyValuePair<string, string>>(line);
                        edited[pair.Key] = pair.Value;
                    }
                }
                var currentHistoryLog = System.IO.File.ReadAllLines(Constants.LogHistory);
                if (0 < currentHistoryLog.Length)
                {
                    var startLine = currentHistoryLog[0];
                    if (startLine.TryParseKey(out var start))
                    {
                        history = new System.Collections.Generic.TimeSeries<AI.Chat.Record>(start);
                        newHistoryLog.Add(startLine);
                        fresh = false;
                        for (var i = 1; i < currentHistoryLog.Length; ++i)
                        {
                            var line = currentHistoryLog[i];
                            var pair = System.Text.Json.JsonSerializer.Deserialize<System.Collections.Generic.KeyValuePair<string, AI.Chat.Record>>(line);
                            if (deleted.Contains(pair.Key))
                            {
                                continue;
                            }
                            if (edited.ContainsKey(pair.Key))
                            {
                                pair = new System.Collections.Generic.KeyValuePair<string, Record>(
                                    pair.Key, new Record
                                    {
                                        Message = edited[pair.Key],
                                        Tags = pair.Value.Tags
                                    });
                                line = System.Text.Json.JsonSerializer.Serialize(
                                    pair,
                                    new System.Text.Json.JsonSerializerOptions
                                    {
                                        Encoder = System.Text.Encodings.Web.JavaScriptEncoder.Create(
                                            System.Text.Unicode.UnicodeRanges.All)
                                    });
                            }
                            history.Add(pair.Key.ParseKey(), pair.Value);
                            newHistoryLog.Add(line);
                        }
                    }
                }
            }
            if (fresh)
            {
                history = new System.Collections.Generic.TimeSeries<Record>();
                newHistoryLog.Add(history.Start.ToKeyString());
            }
            System.IO.File.WriteAllLines(Constants.LogHistory, newHistoryLog);
            System.IO.File.WriteAllText(Constants.LogDeleted, string.Empty);
            System.IO.File.WriteAllText(Constants.LogEdited, string.Empty);
            return history;
        }
    }
}
