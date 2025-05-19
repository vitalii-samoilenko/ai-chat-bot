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
            foreach (var entry in (System.Collections.Generic.IEnumerable<System.Collections.Generic.Entry<Record>>)_records)
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
        public System.Collections.Generic.List<System.DateTime> Remove(System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var removed = new System.Collections.Generic.List<System.DateTime>();
            foreach (var key in keys)
            {
                if (!_records.TryGet(key, out var entry))
                {
                    continue;
                }
                var record = entry.Value;
                if (_records.Remove(key))
                {
                    removed.Add(key);
                    foreach (var tag in record.Tags)
                    {
                        Remove(key, tag);
                    }
                }
            }
            return removed;
        }
        public void Clear()
        {
            _records.Clear();
            _indexes.Clear();
        }
        public System.Collections.Generic.IEnumerable<System.DateTime> Find(System.DateTime fromKey, System.DateTime toKey, System.Collections.Generic.IEnumerable<string> tags)
        {
            var enumerators = new System.Collections.Generic.List<System.Collections.Generic.IEnumerator<System.Collections.Generic.Entry<Record>>>();
            try
            {
                foreach (var tag in tags)
                {
                    if (!_indexes.TryGetValue(tag, out var index))
                    {
                        yield break;
                    }
                    var enumerator = index.Find(fromKey, toKey).GetEnumerator();
                    if (!enumerator.MoveNext())
                    {
                        yield break;
                    }
                    enumerators.Add(enumerator);
                }
                if (!(0 < enumerators.Count))
                {
                    var enumerator = _records.Find(fromKey, toKey).GetEnumerator();
                    if (!enumerator.MoveNext())
                    {
                        yield break;
                    }
                    enumerators.Add(enumerator);
                }
                var firstEnumerator = enumerators[0];
                do
                {
                    var entryKey = firstEnumerator.Current.Key;
                    var adjusted = true;
                    for (var i = 1; i < enumerators.Count; ++i)
                    {
                        var enumerator = enumerators[i];
                        var currentKey = enumerator.Current.Key;
                        while (currentKey < entryKey)
                        {
                            if (!enumerator.MoveNext())
                            {
                                yield break;
                            }
                        }
                        if (entryKey < currentKey)
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
                for (var i = 0; i < enumerators.Count; ++i)
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
            var record = entry.Value;
            if (record.Message == message)
            {
                return false;
            }
            entry.Value = new Record
            {
                Message = message,
                Tags = record.Tags,
            };
            Indexate(entry);
            return true;
        }
        public System.Collections.Generic.List<System.DateTime> Tag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var tagged = new System.Collections.Generic.List<System.DateTime>();
            foreach (var key in keys)
            {
                if (!_records.TryGet(key, out var entry))
                {
                    continue;
                }
                var record = entry.Value;
                var tags = record.Tags;
                if (tags.Contains(tag))
                {
                    continue;
                }
                tagged.Add(key);
                tags.Add(tag);
                Indexate(entry);
            }
            return tagged;
        }
        public System.Collections.Generic.List<System.DateTime> Untag(string tag, System.Collections.Generic.IEnumerable<System.DateTime> keys)
        {
            var untagged = new System.Collections.Generic.List<System.DateTime>();
            foreach (var key in keys)
            {
                if (!_records.TryGet(key, out var entry))
                {
                    continue;
                }
                var record = entry.Value;
                var tags = record.Tags;
                if (!tags.Remove(tag))
                {
                    continue;
                }
                untagged.Add(key);
                Remove(key, tag);
            }
            return untagged;
        }

        private void Indexate(System.Collections.Generic.Entry<Record> entry)
        {
            var key = entry.Key;
            var record = entry.Value;
            foreach (var tag in record.Tags)
            {
                if (!_indexes.TryGetValue(tag, out var index))
                {
                    index = new System.Collections.Generic.TimeSeries<Record>(_records.Start);
                    _indexes[tag] = index;
                }
                if (index.TryGet(key, out var existing))
                {
                    existing.Value = record;
                }
                else
                {
                    index.Add(key, record);
                }
            }
        }
        private void Remove(System.DateTime key, string tag)
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
