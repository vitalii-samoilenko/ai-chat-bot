namespace System.Collections.Generic
{
    public struct EntryKey
    {
        private const string Format = "HHmmss";

        public static bool TryParse(string s, out EntryKey result)
        {
            result = default;
            return Format.Length < s.Length
                && DateTime.TryParseExact(s.Substring(0, Format.Length), Format, null, Globalization.DateTimeStyles.None, out result.Point)
                && int.TryParse(s.Substring(Format.Length), out result.Index)
                && !(result.Index < 0);
        }

        public DateTime Point;
        public int Index;

        public static bool operator <(EntryKey lhs, EntryKey rhs)
        {
            var lhsPoint = new DateTime(1, 1, 1, lhs.Point.Hour, lhs.Point.Minute, lhs.Point.Second);
            var rhsPoint = new DateTime(1, 1, 1, rhs.Point.Hour, rhs.Point.Minute, rhs.Point.Second);
            return lhsPoint < rhsPoint
                || !(rhsPoint < lhsPoint)
                    && lhs.Index < rhs.Index;
        }
        public static bool operator >(EntryKey lhs, EntryKey rhs)
        {
            return rhs < lhs;
        }
        public override string ToString()
        {
            return $"{Point.ToString(Format)}{Index}";
        }
    }

    public class Entry<T>
    {
        public EntryKey Key { get; internal set; }
        public T Item { get; internal set; }
        public Entry<T> Previous { get; internal set; }
        public Entry<T> Next { get; internal set; }
    }

    public class TimeSeries<T> : IEnumerable<T>
    {
        private List<Entry<T>>[][][] _buckets;
        private Entry<T> _head;
        private Entry<T> _tail;

        public IEnumerable<Entry<T>> Entries
        {
            get
            {
                for (var current = _head; current != null; current = current.Next)
                {
                    yield return current;
                }
            }
        }

        public Entry<T> Add(T item)
        {
            var now = DateTime.UtcNow;
            if (_buckets == null)
            {
                _buckets = new List<Entry<T>>[24][][];
            }
            if (_buckets[now.Hour] == null)
            {
                _buckets[now.Hour] = new List<Entry<T>>[60][];
            }
            if (_buckets[now.Hour][now.Minute] == null)
            {
                _buckets[now.Hour][now.Minute] = new List<Entry<T>>[60];
            }
            if (_buckets[now.Hour][now.Minute][now.Second] == null)
            {
                _buckets[now.Hour][now.Minute][now.Second] = new List<Entry<T>>();
            }
            var bucket = _buckets[now.Hour][now.Minute][now.Second];
            var index = bucket.Count;

            var entry = new Entry<T>
            {
                Key = new EntryKey
                {
                    Point = new DateTime(1, 1, 1, now.Hour, now.Minute, now.Second),
                    Index = index
                },
                Item = item
            };
            bucket.Add(entry);
            if (_tail == null)
            {
                _head = entry;
                _tail = entry;
            }
            else
            {
                _tail.Next = entry;
                entry.Previous = _tail;
                _tail = entry;
            }

            return entry;
        }
        public bool Remove(EntryKey key)
        {
            var result = false;

            if (_buckets == null
                || _buckets[key.Point.Hour] == null
                || _buckets[key.Point.Hour][key.Point.Minute] == null
                || _buckets[key.Point.Hour][key.Point.Minute][key.Point.Second] == null)
            {
                return result;
            }
            var bucket = _buckets[key.Point.Hour][key.Point.Minute][key.Point.Second];

            if (key.Index < 0
                || !(key.Index < bucket.Count))
            {
                return result;
            }
            var entry = bucket[key.Index];

            bucket[key.Index] = null;
            if (entry.Previous != null)
            {
                entry.Previous.Next = entry.Next;
            }
            if (entry.Next != null)
            {
                entry.Next.Previous = entry.Previous;
            }
            if (entry == _tail)
            {
                _tail = entry.Previous;
            }
            if (entry == _head)
            {
                _head = entry.Next;
            }
            result = true;

            if (_tail == null)
            {
                _buckets = null;
                return result;
            }
            if (entry.Previous == null
                || entry.Key.Point.Hour != entry.Previous.Key.Point.Hour)
            {
                if (entry.Next == null
                    || entry.Key.Point.Hour != entry.Next.Key.Point.Hour)
                {
                    _buckets[entry.Key.Point.Hour] = null;
                    return result;
                }
            }
            if (entry.Previous == null
                || entry.Key.Point.Hour != entry.Previous.Key.Point.Hour
                || entry.Key.Point.Minute != entry.Previous.Key.Point.Minute)
            {
                if (entry.Next == null
                    || entry.Key.Point.Hour != entry.Next.Key.Point.Hour
                    || entry.Key.Point.Minute != entry.Next.Key.Point.Minute)
                {
                    _buckets[entry.Key.Point.Hour][entry.Key.Point.Minute] = null;
                    return result;
                }
            }
            foreach (var _ in bucket)
            {
                if (_ != null)
                {
                    return result;
                }
            }
            _buckets[key.Point.Hour][key.Point.Minute][key.Point.Second] = null;
            return result;
        }
        public void Clear()
        {
            _buckets = null;
            _head = null;
            _tail = null;
        }
        public bool TryGet(EntryKey key, out Entry<T> entry)
        {
            entry = default;

            if (_buckets == null
                || _buckets[key.Point.Hour] == null
                || _buckets[key.Point.Hour][key.Point.Minute] == null
                || _buckets[key.Point.Hour][key.Point.Minute][key.Point.Second] == null)
            {
                return false;
            }
            var bucket = _buckets[key.Point.Hour][key.Point.Minute][key.Point.Second];

            if (key.Index < 0
                || !(key.Index < bucket.Count))
            {
                return false;
            }
            entry = bucket[key.Index];

            return true;
        }
        public IEnumerable<Entry<T>> Find(EntryKey fromKey, EntryKey toKey)
        {
            if (_buckets == null)
            {
                yield break;
            }
            if (toKey < fromKey)
            {
                foreach (var key in Find(fromKey, new EntryKey
                {
                    Point = new DateTime(1, 1, 1, 23, 59, 59),
                    Index = int.MaxValue
                }))
                {
                    yield return key;
                }
                fromKey = new EntryKey
                {
                    Point = new DateTime(1, 1, 1, 0, 0, 0),
                    Index = 0
                };
            }
            bool TryFind(EntryKey key, out Entry<T> entry)
            {
                entry = default;
                bool adjusted = true;
                for (var hour = key.Point.Hour; hour < 24; ++hour, adjusted = false)
                {
                    if (_buckets[hour] == null)
                    {
                        continue;
                    }
                    for (var minute = adjusted ? key.Point.Minute : 0; minute < 60; ++minute, adjusted = false)
                    {
                        if (_buckets[hour][minute] == null)
                        {
                            continue;
                        }
                        for (var second = adjusted ? key.Point.Second : 0; second < 60; ++second, adjusted = false)
                        {
                            if (_buckets[hour][minute][second] == null)
                            {
                                continue;
                            }
                            var bucket = _buckets[hour][minute][second];
                            for (var i = adjusted ? key.Index : 0; i < bucket.Count; ++i)
                            {
                                if (bucket[i] == null)
                                {
                                    continue;
                                }
                                entry = bucket[i];
                                return true;
                            }
                        }
                    }
                }
                return false;
            }
            if (TryFind(fromKey, out var current))
            {
                for (; current != null && !(toKey < current.Key) && !(current.Key < fromKey); current = current.Next)
                {
                    yield return current;
                }
            }
        }

        public IEnumerator<T> GetEnumerator()
        {
            foreach (var entry in Entries)
            {
                yield return entry.Item;
            }
        }
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
    }
}
