namespace Microsoft.AspNetCore.Authentication.ApiKey
{
    public class Client
    {
        public string Key { get; set; }
        public string Name { get; set; }
        public System.Collections.Generic.List<string> Roles { get; set; }
    }

    public class Options : AuthenticationSchemeOptions
    {
        public System.Collections.Generic.List<Client> Clients { get; set; }
    }
}
