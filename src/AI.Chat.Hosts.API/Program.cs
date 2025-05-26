using OpenTelemetry;
using OpenTelemetry.Metrics;
using OpenTelemetry.Trace;

var builder = WebApplication.CreateBuilder(args);

builder.Logging.ClearProviders();
builder.Logging.AddConsole();
builder.Logging.AddOpenTelemetry(logging =>
{
    logging.IncludeFormattedMessage = true;
    logging.IncludeScopes = true;
});
builder.Configuration.ConfigureAIChat();
builder.Services.AddOpenTelemetry()
    .WithMetrics(metrics =>
    {
        metrics.AddMeter("System.Runtime");
        metrics.AddAspNetCoreInstrumentation();
        metrics.AddHttpClientInstrumentation();

        metrics.AddMeter(AI.Chat.Diagnostics.Meters.Adapters.Name);
        metrics.AddMeter(AI.Chat.Diagnostics.Meters.Bots.Name);
        metrics.AddMeter(AI.Chat.Diagnostics.Meters.Scopes.Name);

        metrics.AddMeter("OpenAI.ChatClient");
        metrics.AddMeter(TwitchLib.Client.Diagnostics.Meters.Client.Name);
    })
    .WithTracing(tracing =>
    {
        tracing.AddAspNetCoreInstrumentation();
        tracing.AddHttpClientInstrumentation();

        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Adapters.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Bots.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Clients.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.CommandExecutors.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Commands.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Filters.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Histories.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Moderators.Name);
        tracing.AddSource(AI.Chat.Diagnostics.ActivitySources.Scopes.Name);

        tracing.AddSource("OpenAI.ChatClient");
        tracing.AddSource(TwitchLib.Client.Diagnostics.ActivitySources.Client.Name);
        tracing.AddSource(GoogleAI.Diagnostics.ActivitySources.Client.Name);
    })
    .UseOtlpExporter();
builder.Services.AddAIChat(builder.Configuration);
builder.Services.AddControllers();
builder.Services.AddOpenApi();
builder.Services.AddHealthChecks();

var app = builder.Build();

if (app.Environment.IsDevelopment())
{
    app.MapOpenApi();
}
app.MapControllers();
app.MapHealthChecks("/health");

await app.RunAsync();
