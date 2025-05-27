namespace Microsoft.Extensions.DependencyInjection
{
    public static class AuthenticationBuilder
    {
        public static Microsoft.AspNetCore.Authentication.AuthenticationBuilder AddApiKey(this Microsoft.AspNetCore.Authentication.AuthenticationBuilder authentication, System.Action<Microsoft.AspNetCore.Authentication.ApiKey.Options> configureOptions)
        {
            authentication.AddScheme<Microsoft.AspNetCore.Authentication.ApiKey.Options, Microsoft.AspNetCore.Authentication.ApiKey.Handler>(Microsoft.AspNetCore.Authentication.ApiKey.Defaults.AuthenticationScheme, null, configureOptions);
            return authentication;
        }
        public static Microsoft.AspNetCore.Authentication.AuthenticationBuilder AddApiKey(this Microsoft.AspNetCore.Authentication.AuthenticationBuilder authentication, string authenticationName,  System.Action<Microsoft.AspNetCore.Authentication.ApiKey.Options> configureOptions)
        {
            authentication.AddScheme<Microsoft.AspNetCore.Authentication.ApiKey.Options, Microsoft.AspNetCore.Authentication.ApiKey.Handler>(authenticationName, null, configureOptions);
            return authentication;
        }
        public static Microsoft.AspNetCore.Authentication.AuthenticationBuilder AddApiKey(this Microsoft.AspNetCore.Authentication.AuthenticationBuilder authentication, string authenticationName, string displayName, System.Action<Microsoft.AspNetCore.Authentication.ApiKey.Options> configureOptions)
        {
            authentication.AddScheme<Microsoft.AspNetCore.Authentication.ApiKey.Options, Microsoft.AspNetCore.Authentication.ApiKey.Handler>(authenticationName, displayName, configureOptions);
            return authentication;
        }
    }
}
