FROM mcr.microsoft.com/dotnet/aspnet:9.0 AS base
USER $APP_UID
WORKDIR /app
EXPOSE 8080

FROM mcr.microsoft.com/dotnet/sdk:9.0 AS restore
COPY ["src/AI.Chat/AI.Chat.csproj", "/src/AI.Chat/"]
COPY ["src/AI.Chat.Diagnostics/AI.Chat.Diagnostics.csproj", "/src/AI.Chat.Diagnostics/"]
COPY ["src/GoogleAI/GoogleAI.csproj", "/src/GoogleAI/"]
COPY ["src/GoogleAI.Diagnostics/GoogleAI.Diagnostics.csproj", "/src/GoogleAI.Diagnostics/"]
COPY ["src/TwitchLib.Client.AuthClient/TwitchLib.Client.AuthClient.csproj", "/src/TwitchLib.Client.AuthClient/"]
COPY ["src/TwitchLib.Client.Diagnostics/TwitchLib.Client.Diagnostics.csproj", "/src/TwitchLib.Client.Diagnostics/"]
COPY ["src/System.Collections.Generic.TimeSeries/System.Collections.Generic.TimeSeries.csproj", "/src/System.Collections.Generic.TimeSeries/"]
COPY ["src/Microsoft.AspNetCore.Authentication.ApiKey/Microsoft.AspNetCore.Authentication.ApiKey.csproj", "/src/Microsoft.AspNetCore.Authentication.ApiKey/"]
COPY ["src/AI.Chat.Adapters.OpenAI/AI.Chat.Adapters.OpenAI.csproj", "/src/AI.Chat.Adapters.OpenAI/"]
COPY ["src/AI.Chat.Adapters.GoogleAI/AI.Chat.Adapters.GoogleAI.csproj", "/src/AI.Chat.Adapters.GoogleAI/"]
COPY ["src/AI.Chat.Clients.Twitch/AI.Chat.Clients.Twitch.csproj", "/src/AI.Chat.Clients.Twitch/"]
COPY ["src/AI.Chat.Clients.Console/AI.Chat.Clients.Console.csproj", "/src/AI.Chat.Clients.Console/"]
COPY ["src/AI.Chat.Host/AI.Chat.Host.csproj", "/src/AI.Chat.Host/"]
COPY ["src/AI.Chat.Hosts.API/AI.Chat.Hosts.API.csproj", "/src/AI.Chat.Hosts.API/"]
RUN --mount=type=cache,id=nuget,target=/root/.nuget/packages \
	dotnet restore "/src/AI.Chat.Hosts.API/AI.Chat.Hosts.API.csproj"

FROM restore AS build
ARG BUILD_CONFIGURATION=Release
COPY ["src/AI.Chat", "/src/AI.Chat"]
COPY ["src/AI.Chat.Diagnostics", "/src/AI.Chat.Diagnostics"]
COPY ["src/GoogleAI", "/src/GoogleAI"]
COPY ["src/GoogleAI.Diagnostics", "/src/GoogleAI.Diagnostics"]
COPY ["src/TwitchLib.Client.AuthClient", "/src/TwitchLib.Client.AuthClient"]
COPY ["src/TwitchLib.Client.Diagnostics", "/src/TwitchLib.Client.Diagnostics"]
COPY ["src/System.Collections.Generic.TimeSeries", "/src/System.Collections.Generic.TimeSeries"]
COPY ["src/Microsoft.AspNetCore.Authentication.ApiKey", "/src/Microsoft.AspNetCore.Authentication.ApiKey"]
COPY ["src/AI.Chat.Adapters.OpenAI", "/src/AI.Chat.Adapters.OpenAI"]
COPY ["src/AI.Chat.Adapters.GoogleAI", "/src/AI.Chat.Adapters.GoogleAI"]
COPY ["src/AI.Chat.Clients.Twitch", "/src/AI.Chat.Clients.Twitch"]
COPY ["src/AI.Chat.Clients.Console", "/src/AI.Chat.Clients.Console"]
COPY ["src/AI.Chat.Host", "/src/AI.Chat.Host"]
COPY ["src/AI.Chat.Hosts.API", "/src/AI.Chat.Hosts.API"]
RUN --mount=type=cache,id=nuget,target=/root/.nuget/packages \
	dotnet build "/src/AI.Chat.Hosts.API/AI.Chat.Hosts.API.csproj" -c $BUILD_CONFIGURATION --no-restore

FROM build AS publish
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=nuget,target=/root/.nuget/packages \
	dotnet publish "/src/AI.Chat.Hosts.API/AI.Chat.Hosts.API.csproj" -c $BUILD_CONFIGURATION -o /app /p:UseAppHost=false --no-restore

FROM base AS install
USER root
RUN apt-get update && apt-get install -y curl
USER $APP_UID

FROM install AS host
COPY --from=publish /app .
ENTRYPOINT ["dotnet", "AI.Chat.Hosts.API.dll"]
HEALTHCHECK --interval=15s --timeout=3s \
	CMD curl -f http://localhost:8080/health || exit 1