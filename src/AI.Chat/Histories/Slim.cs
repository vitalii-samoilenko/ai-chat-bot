namespace AI.Chat.Histories
{
    public class Slim : IHistory
    {
        private readonly System.Collections.Generic.TimeSeries<Record> _records;
        private readonly System.Collections.Generic.Dictionary<string, System.Collections.Generic.TimeSeries<Record>> _indexes;

        public Slim()
            : this(new System.Collections.Generic.TimeSeries<Record>())
        {

        }
        public Slim(System.Collections.Generic.TimeSeries<Record> records)
        {
            _records = records;
            _indexes = new System.Collections.Generic.Dictionary<string, System.Collections.Generic.TimeSeries<Record>>(System.StringComparer.OrdinalIgnoreCase);
            foreach (var entry in _records.Entries)
            {
                Indexate(entry);
            }
        }

        public System.DateTime Add(Record record)
        {
            var entry = _records.Add(record);
            Indexate(entry);
            return entry.Key;
        }
        public void Remove(params System.DateTime[] keys)
        {
            foreach (var key in keys)
            {
                if (!_records.TryGet(key, out var entry))
                {
                    continue;
                }
                _records.Remove(key);
                foreach (var tag in entry.Value.Tags)
                {
                    var index = _indexes[tag];
                    if (1 < index.Count)
                    {
                        index.Remove(key);
                    }
                    else
                    {
                        _indexes.Remove(tag);
                    }                    
                }
            }
        }
        public void Clear()
        {
            _records.Clear();
            _indexes.Clear();
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, params string[] tags)
        {
            if (0 < tags.Length)
            {
                var enumerators = new System.Collections.Generic.IEnumerator<System.Collections.Generic.Entry<Record>>[tags.Length];
                try
                {
                    for (var i = 0; i < tags.Length; ++i)
                    {
                        var tag = tags[i];
                        if (!_indexes.TryGetValue(tag, out var index))
                        {
                            yield break;
                        }
                        var enumerator = index.Find(fromKey, toKey).GetEnumerator();
                        enumerators[i] = enumerator;
                        if (!enumerator.MoveNext())
                        {
                            yield break;
                        }
                    }
                    var firstEnumerator = enumerators[0];
                    do
                    {
                        var entryKey = firstEnumerator.Current.Key;
                        var adjusted = true;
                        for (var i = 1; i < enumerators.Length; ++i)
                        {
                            var enumerator = enumerators[i];
                            while (enumerator.Current.Key < entryKey)
                            {
                                if (!enumerator.MoveNext())
                                {
                                    yield break;
                                }
                            }
                            if (entryKey < enumerator.Current.Key)
                            {
                                adjusted = false;
                                break;
                            }
                        }
                        if (adjusted)
                        {
                            yield return entryKey;
                        }
                    }
                    while (firstEnumerator.MoveNext());
                }
                finally
                {
                    for (var i = 0; i < enumerators.Length; ++i)
                    {
                        var enumerator = enumerators[i];
                        if (enumerator == null)
                        {
                            break;
                        }
                        enumerator.Dispose();
                    }
                }
            }
            else
            {
                foreach (var entry in _records.Find(fromKey, toKey))
                {
                    yield return entry.Key;
                }
            }
        }
        public bool TryGet(System.DateTime key, out Record record)
        {
            record = default;
            if (_records.TryGet(key, out var entry))
            {
                record = entry.Value;
                return true;
            }
            return false;
        }
        public bool TryEdit(System.DateTime key, string message)
        {
            if (!_records.TryGet(key, out var entry))
            {
                return false;
            }
            entry.Value = new Record
            {
                Message = message,
                Tags = entry.Value.Tags,
            };
            foreach (var tag in entry.Value.Tags)
            {
                var index = _indexes[tag];
                index[key].Value = entry.Value;
            }
            return true;
        }

        private void Indexate(System.Collections.Generic.Entry<Record> entry)
        {
            foreach (var tag in entry.Value.Tags)
            {
                if (!_indexes.TryGetValue(tag, out var index))
                {
                    index = new System.Collections.Generic.TimeSeries<Record>(_records.Start);
                    _indexes[tag] = index;
                }
                index.Add(entry.Key, entry.Value);
            }
        }
    }
}
