using Microsoft.Extensions.Options;

var builder = WebApplication.CreateBuilder(args);

builder.Services.Configure<AI.Chat.Hosts.API.Moderator.Options.Client>(
    builder.Configuration.GetSection("Client"));

builder.Services.AddControllers();
builder.Services.AddHttpClient<AI.Chat.Hosts.API.IClient, AI.Chat.Hosts.API.Client>(
    (serviceProvider, httpClient) =>
    {
        var options = serviceProvider
            .GetRequiredService<IOptions<AI.Chat.Hosts.API.Moderator.Options.Client>>()
            .Value;

        httpClient.BaseAddress = options.BaseAddress;
    })
    .AddHttpMessageHandler(
        serviceProvider =>
        {
            var options = serviceProvider
                .GetRequiredService<IOptions<AI.Chat.Hosts.API.Moderator.Options.Client>>()
                .Value;

            return new System.Net.Http.AI.Chat.Hosts.API.ApiKeyHandler(
                options.ApiKey);
        });

var app = builder.Build();

app.UseDefaultFiles();
app.MapStaticAssets();
app.UseAuthorization();
app.MapControllers();
app.MapFallbackToFile("/index.html");

await app.RunAsync();
