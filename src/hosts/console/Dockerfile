FROM ubuntu AS base
USER $APP_UID
WORKDIR /home/$APP_UID

FROM gcc AS prepare
WORKDIR /root
COPY ["CMakeLists.txt", "CMakeLists.txt"]
COPY ["third_party/boost", "third_party/boost"]
COPY ["third_party/eboost", "third_party/eboost"]
COPY ["third_party/openai", "third_party/openai"]
COPY ["third_party/opentelemetry-cpp", "third_party/opentelemetry-cpp"]
COPY ["third_party/twitch", "third_party/twitch"]
COPY ["src/adapters/openai", "src/adapters/openai"]
COPY ["src/adapters/telemetry", "src/adapters/telemetry"]
COPY ["src/hosts/console", "src/hosts/console"]
RUN apt-get update && apt-get install -y cmake

FROM prepare AS configure
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake -S . -B build

FROM configure AS build
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake --build build --target AI.Chat.Hosts.Console --config $BUILD_CONFIGURATION

FROM build AS install
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake --install build --config $BUILD_CONFIGURATION --prefix install

FROM base AS host
COPY --from=install /root/install /usr/local
ENTRYPOINT ["/usr/local/bin/ai-chat-hosts-console"]
