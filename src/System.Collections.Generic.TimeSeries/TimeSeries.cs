namespace System.Collections.Generic
{
    public class Entry<T>
    {
        public DateTime Key { get; internal set; }
        public T Value { get; set; }
        public Entry<T> Previous { get; internal set; }
        public Entry<T> Next { get; internal set; }
    }

    public class EntryComparer<T> : IComparer<Entry<T>>
    {
        public int Compare(Entry<T> x, Entry<T> y)
        {
            return x.Key.CompareTo(y.Key);
        }
    }

    public class TimeSeries<T> : IEnumerable<T>, IEnumerable<Entry<T>>
    {
        private static readonly IComparer<Entry<T>> EntryComparer = new EntryComparer<T>();

        private readonly DateTime _start;
        private readonly List<List<Entry<T>>[][][][][]> _buckets;
        private Entry<T> _head;
        private Entry<T> _tail;
        private int _count;

        public TimeSeries()
            : this(DateTime.UtcNow)
        {

        }
        public TimeSeries(DateTime start)
        {
            _start = start.ToUniversalTime();
            _buckets = new List<List<Entry<T>>[][][][][]>();
            _head = null;
            _tail = null;
            _count = 0;
        }

        public DateTime Start
        {
            get
            {
                return _start;
            }
        }
        public int Count
        {
            get
            {
                return _count;
            }
        }

        public Entry<T> this[DateTime key]
        {
            get
            {
                if (!TryGet(key, out var entry))
                {
                    throw new ArgumentException("Entry does not exist", nameof(key));
                }

                return entry;
            }
        }
        public Entry<T> Add(T item)
        {
            return Add(DateTime.UtcNow, item);
        }

        public Entry<T> Add(DateTime key, T value)
        {
            key = key.ToUniversalTime();
            if (key < _start)
            {
                throw new ArgumentOutOfRangeException(nameof(key));
            }
            var year = key.Year - _start.Year;
            var month = key.Month - 1;
            var day = key.Day - 1;

            var entry = new Entry<T>
            {
                Key = key,
                Value = value
            };
            if (_tail == null)
            {
                _head = entry;
                _tail = entry;
            }
            else if (TryFind(key, out var next))
            {
                if (!(key < next.Key))
                {
                    throw new ArgumentException("Entry already exists", nameof(key));
                }
                next.Previous.Next = entry;
                entry.Previous = next.Previous;
                entry.Next = next;
                next.Previous = entry;
            }
            else
            {
                entry.Previous = _tail;
                _tail.Next = entry;
                _tail = entry;
            }

            while (_buckets.Count < year + 1)
            {
                _buckets.Add(null);
            }
            if (_buckets[year] == null)
            {
                _buckets[year] = new List<Entry<T>>[12][][][][];
            }
            if (_buckets[year][month] == null)
            {
                _buckets[year][month] = new List<Entry<T>>[31][][][];
            }
            if (_buckets[year][month][day] == null)
            {
                _buckets[year][month][day] = new List<Entry<T>>[24][][];
            }
            var dayBucket = _buckets[year][month][day];

            if (dayBucket[key.Hour] == null)
            {
                dayBucket[key.Hour] = new List<Entry<T>>[60][];
            }
            if (dayBucket[key.Hour][key.Minute] == null)
            {
                dayBucket[key.Hour][key.Minute] = new List<Entry<T>>[60];
            }
            if (dayBucket[key.Hour][key.Minute][key.Second] == null)
            {
                dayBucket[key.Hour][key.Minute][key.Second] = new List<Entry<T>>();
            }
            var secondBucket = dayBucket[key.Hour][key.Minute][key.Second];

            var index = ~secondBucket.BinarySearch(entry, EntryComparer);
            secondBucket.Insert(index, entry);
            ++_count;

            return entry;
        }
        public bool Remove(DateTime key)
        {
            key = key.ToUniversalTime();
            if (key < _start)
            {
                return false;
            }
            var year = key.Year - _start.Year;
            var month = key.Month - 1;
            var day = key.Day - 1;

            if (!(year < _buckets.Count)
                || _buckets[year] == null
                || _buckets[year][month] == null
                || _buckets[year][month][day] == null)
            {
                return false;
            }
            var dayBucket = _buckets[year][month][day];

            if (dayBucket[key.Hour] == null
                || dayBucket[key.Hour][key.Minute] == null
                || dayBucket[key.Hour][key.Minute][key.Second] == null)
            {
                return false;
            }
            var secondBucket = dayBucket[key.Hour][key.Minute][key.Second];

            var entry = new Entry<T>
            {
                Key = key
            };
            var index = secondBucket.BinarySearch(entry, EntryComparer);
            if (index < 0)
            {
                return false;
            }
            entry = secondBucket[index];

            var previousKey = key;
            if (entry.Previous != null)
            {
                entry.Previous.Next = entry.Next;
                previousKey = entry.Previous.Key;
            }
            var nextKey = key;
            if (entry.Next != null)
            {
                entry.Next.Previous = entry.Previous;
                nextKey = entry.Next.Key;
            }
            if (entry == _tail)
            {
                _tail = entry.Previous;
            }
            if (entry == _head)
            {
                _head = entry.Next;
            }
            secondBucket.RemoveAt(index);
            --_count;
            var previousBucketIsDifferent = previousKey.Year < key.Year;
            var nextBucketIsDifferent = key.Year < nextKey.Year;
            if (previousBucketIsDifferent && nextBucketIsDifferent)
            {
                _buckets[key.Year] = null;
                return true;
            }
            previousBucketIsDifferent |= previousKey.Month < key.Month;
            nextBucketIsDifferent |= key.Month < nextKey.Month;
            if (previousBucketIsDifferent && nextBucketIsDifferent)
            {
                _buckets[key.Year][key.Month] = null;
                return true;
            }
            previousBucketIsDifferent |= previousKey.Day < key.Day;
            nextBucketIsDifferent |= key.Day < nextKey.Day;
            if (previousBucketIsDifferent && nextBucketIsDifferent)
            {
                _buckets[key.Year][key.Month][key.Day] = null;
                return true;
            }
            previousBucketIsDifferent |= previousKey.Hour < key.Hour;
            nextBucketIsDifferent |= key.Hour < nextKey.Hour;
            if (previousBucketIsDifferent && nextBucketIsDifferent)
            {
                dayBucket[key.Hour] = null;
                return true;
            }
            previousBucketIsDifferent |= previousKey.Minute < key.Minute;
            nextBucketIsDifferent |= key.Minute < nextKey.Minute;
            if (previousBucketIsDifferent && nextBucketIsDifferent)
            {
                dayBucket[key.Hour][key.Minute] = null;
                return true;
            }
            if (secondBucket.Count < 1)
            {
                dayBucket[key.Hour][key.Minute][key.Second] = null;
            }

            return true;
        }
        public void Clear()
        {
            _buckets.Clear();
            _head = null;
            _tail = null;
            _count = 0;
        }
        public bool TryGet(DateTime key, out Entry<T> entry)
        {
            key = key.ToUniversalTime();
            entry = default;
            if (key < _start)
            {
                return false;
            }
            var year = key.Year - _start.Year;
            var month = key.Month - 1;
            var day = key.Day - 1;

            if (!(year < _buckets.Count)
                || _buckets[year] == null
                || _buckets[year][month] == null
                || _buckets[year][month][day] == null)
            {
                return false;
            }
            var dayBucket = _buckets[year][month][day];

            if (dayBucket[key.Hour] == null
                || dayBucket[key.Hour][key.Minute] == null
                || dayBucket[key.Hour][key.Minute][key.Second] == null)
            {
                return false;
            }
            var secondBucket = dayBucket[key.Hour][key.Minute][key.Second];

            entry = new Entry<T>
            {
                Key = key
            };
            var index = secondBucket.BinarySearch(entry, EntryComparer);
            if (index < 0)
            {
                return false;
            }
            entry = secondBucket[index];

            return true;
        }
        public IEnumerable<Entry<T>> Find(DateTime fromKey, DateTime toKey)
        {
            if (toKey < fromKey)
            {
                throw new ArgumentException("Invalid time range");
            }
            fromKey = fromKey.ToUniversalTime();
            toKey = toKey.ToUniversalTime();
            if (toKey < _start)
            {
                yield break;
            }
            if (fromKey < _start)
            {
                fromKey = _start;
            }
            if (!TryFind(fromKey, out var current))
            {
                yield break;
            }
            for (; current != null && !(toKey < current.Key); current = current.Next)
            {
                yield return current;
            }
        }

        IEnumerator<Entry<T>> IEnumerable<Entry<T>>.GetEnumerator()
        {
            for (var current = _head; current != null; current = current.Next)
            {
                yield return current;
            }
        }
        public IEnumerator<T> GetEnumerator()
        {
            foreach (var entry in (IEnumerable<Entry<T>>)this)
            {
                yield return entry.Value;
            }
        }
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        private bool TryFind(DateTime key, out Entry<T> entry)
        {
            entry = default;
            var adjusted = true;
            for (var year = key.Year - _start.Year; year < _buckets.Count; ++year, adjusted = false)
            {
                if (_buckets[year] == null)
                {
                    continue;
                }
                for (var month = adjusted ? key.Month - 1 : 0; month < 12; ++month, adjusted = false)
                {
                    if (_buckets[year][month] == null)
                    {
                        continue;
                    }
                    for (var day = adjusted ? key.Day - 1 : 0; day < 31; ++day, adjusted = false)
                    {
                        if (_buckets[year][month][day] == null)
                        {
                            continue;
                        }
                        var dayBucket = _buckets[year][month][day];
                        for (var hour = adjusted ? key.Hour : 0; hour < 24; ++hour, adjusted = false)
                        {
                            if (dayBucket[hour] == null)
                            {
                                continue;
                            }
                            for (var minute = adjusted ? key.Minute : 0; minute < 60; ++minute, adjusted = false)
                            {
                                if (dayBucket[hour][minute] == null)
                                {
                                    continue;
                                }
                                for (var second = adjusted ? key.Second : 0; second < 60; ++second, adjusted = false)
                                {
                                    if (dayBucket[hour][minute][second] == null)
                                    {
                                        continue;
                                    }
                                    var secondBucket = dayBucket[hour][minute][second];

                                    entry = new Entry<T>
                                    {
                                        Key = adjusted
                                            ? key
                                            : new DateTime(year + 1, month + 1, day + 1, hour, minute, second, 0, DateTimeKind.Utc)
                                    };
                                    var index = secondBucket.BinarySearch(entry, EntryComparer);
                                    if (index < 0)
                                    {
                                        index = ~index;
                                        if (!(index < secondBucket.Count))
                                        {
                                            continue;
                                        }
                                    }
                                    entry = secondBucket[index];
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
            return false;
        }
    }
}
