FROM ubuntu AS base
USER $APP_UID
WORKDIR /app

FROM gcc AS restore
RUN apt-get update && apt-get install -y cmake libssl-dev

FROM restore AS init
COPY ["CMakeLists.txt", "CMakeLists.txt"]
COPY ["libs/boost", "/libs/boost"]
COPY ["src/eboost", "/src/eboost"]
COPY ["src/OpenAI", "/src/OpenAI"]
COPY ["src/Twitch", "/src/Twitch"]
COPY ["src/AI.Chat.Hosts.Console", "/src/AI.Chat.Hosts.Console"]
WORKDIR /build
RUN --mount=type=cache,id=build,target=/build \
    cmake ../

FROM init AS build
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=/build \
    cmake --build . --config $BUILD_CONFIGURATION

FROM build AS install
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=/build \
    cmake --install . --config $BUILD_CONFIGURATION --prefix /install

FROM base AS host
COPY --from=install /install .
ENTRYPOINT ["bin/AI.Chat.Hosts.Console"]
