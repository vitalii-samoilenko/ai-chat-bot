﻿using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using NLog.Extensions.Logging;

var builder = Host.CreateApplicationBuilder(args);

builder.Logging.ClearProviders();
builder.Logging.AddNLog();
builder.Configuration.ConfigureAIChat();
builder.Services.AddAIChat(builder.Configuration, builder.Environment);

using var host = builder.Build();

await host.RunAsync();
