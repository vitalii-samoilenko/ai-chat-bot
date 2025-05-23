﻿namespace AI.Chat.Options.GoogleAI
{
    public class Model
    {
        public string Name { get; set; }
        public global::GoogleAI.Models.GenerationConfig Generation { get; set; }
    }
    public class Client
    {
        public System.Uri BaseAddress { get; set; }
        public string ApiKey { get; set; }
        public System.Collections.Generic.List<System.TimeSpan> Retries { get; set; }
    }
    public class Cache
    {
        public int Tokens { get; set; }
        public string Ttl { get; set; }
        public System.TimeSpan Slide { get; set; }
        public string Name { get; set; }
        public System.DateTime Until {  get; set; }
        public System.DateTime Key { get; set; }
    }
    public class Adapter : Options.Adapter
    {
        public Client Client { get; set; }
        public Cache Cache { get; set; }
        public Model Model { get; set; }
    }
}
