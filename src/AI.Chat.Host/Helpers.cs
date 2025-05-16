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
            System.IO.File.WriteAllText(Constants.LogTags, string.Empty);
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
        public static void TagLog(string tag, params System.DateTime[] keys)
        {
            AppendTagsLog($"+{tag}", keys);
        }
        public static void UntagLog(string tag, params System.DateTime[] keys)
        {
            AppendTagsLog($"-{tag}", keys);
        }
        private static void AppendTagsLog(string tag, System.DateTime[] keys)
        {
            var tagged = new string[keys.Length];
            for (int i = 0; i < keys.Length; ++i)
            {
                tagged[i] = keys[i].ToKeyString();
            }
            System.IO.File.AppendAllLines(
                Constants.LogTags,
                new[]
                {
                    System.Text.Json.JsonSerializer.Serialize(
                        new System.Collections.Generic.KeyValuePair<string, string[]>(
                            tag, tagged))
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
                var edited = new System.Collections.Generic.Dictionary<string, string>(
                    System.StringComparer.OrdinalIgnoreCase);
                if (System.IO.File.Exists(Constants.LogEdited))
                {
                    var editedLog = System.IO.File.ReadAllLines(Constants.LogEdited);
                    foreach (var line in editedLog)
                    {
                        var pair = System.Text.Json.JsonSerializer.Deserialize<System.Collections.Generic.KeyValuePair<string, string>>(line);
                        edited[pair.Key] = pair.Value;
                    }
                }
                var tagged = new System.Collections.Generic.Dictionary<string, System.Collections.Generic.List<string>>(
                    System.StringComparer.OrdinalIgnoreCase);
                if (System.IO.File.Exists(Constants.LogTags))
                {
                    var tagsLog = System.IO.File.ReadAllLines(Constants.LogTags);
                    foreach (var line in tagsLog)
                    {
                        var pair = System.Text.Json.JsonSerializer.Deserialize<System.Collections.Generic.KeyValuePair<string, string[]>>(line);
                        foreach (var key in pair.Value)
                        {
                            if (tagged.ContainsKey(key))
                            {
                                tagged[key].Add(pair.Key);
                            }
                            else
                            {
                                tagged[key] = new System.Collections.Generic.List<string>
                                {
                                    pair.Key
                                };
                            }
                        }
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
                            var key = pair.Key;
                            var record = pair.Value;
                            var modified = false;
                            if (deleted.Contains(key))
                            {
                                continue;
                            }
                            if (tagged.ContainsKey(key))
                            {
                                var tags = new System.Collections.Generic.List<string>(record.Tags);
                                foreach (var action in tagged[key])
                                {
                                    var tag = action.Substring(1);
                                    if (action[0] == '-')
                                    {
                                        tags.Remove(tag);
                                    }
                                    else
                                    {
                                        tags.Add(tag);
                                    }
                                }
                                record.Tags = tags.ToArray();
                                modified = true;
                            }
                            if (record.IsModerated())
                            {
                                continue;
                            }
                            if (edited.ContainsKey(key))
                            {
                                record.Message = edited[key];
                                modified = true;
                            }
                            if (modified)
                            {
                                line = System.Text.Json.JsonSerializer.Serialize(
                                    new System.Collections.Generic.KeyValuePair<string, Record>(
                                        key, record),
                                    new System.Text.Json.JsonSerializerOptions
                                    {
                                        Encoder = System.Text.Encodings.Web.JavaScriptEncoder.Create(
                                            System.Text.Unicode.UnicodeRanges.All)
                                    });
                            }
                            history.Add(key.ParseKey(), record);
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
            System.IO.File.WriteAllText(Constants.LogTags, string.Empty);
            return history;
        }
    }
}
