using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace Microsoft.AspNetCore.Authentication.ApiKey
{
    public class Handler : AuthenticationHandler<Options>
    {
        public Handler(IOptionsMonitor<Options> options, ILoggerFactory logger, System.Text.Encodings.Web.UrlEncoder encoder, ISystemClock clock)
            : base(options, logger, encoder, clock)
        {

        }

        protected override System.Threading.Tasks.Task<AuthenticateResult> HandleAuthenticateAsync()
        {
            if (!Context.Request.Headers.TryGetValue("X-API-KEY", out var values))
            {
                return System.Threading.Tasks.Task.FromResult(
                    AuthenticateResult.NoResult());
            }
            var apiKey = values.ToString();
            foreach (var client in Options.Clients)
            {
                if (!client.Key.Equals(apiKey, System.StringComparison.OrdinalIgnoreCase))
                {
                    continue;
                }
                var identity = new System.Security.Claims.ClaimsIdentity(Scheme.Name);
                identity.AddClaim(new System.Security.Claims.Claim(
                    identity.NameClaimType, client.Name, null, Options.ClaimsIssuer));
                foreach (var role in client.Roles)
                {
                    identity.AddClaim(new System.Security.Claims.Claim(
                        identity.RoleClaimType, role, null, Options.ClaimsIssuer));
                }
                var principal = new System.Security.Claims.ClaimsPrincipal(identity);
                var ticket = new AuthenticationTicket(principal, Scheme.Name);
                return System.Threading.Tasks.Task.FromResult(
                    AuthenticateResult.Success(ticket));
            }
            return System.Threading.Tasks.Task.FromResult(
                AuthenticateResult.Fail("API key is invalid"));
        }
    }
}
