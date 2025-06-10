FROM ubuntu AS base
USER $APP_UID
WORKDIR /home/$APP_UID

FROM gcc AS prepare
WORKDIR /root
COPY ["CMakeLists.txt", "CMakeLists.txt"]
COPY ["libs/boost", "libs/boost"]
COPY ["src/eboost", "src/eboost"]
COPY ["src/OpenAI", "src/OpenAI"]
COPY ["src/Twitch", "src/Twitch"]
COPY ["src/AI.Chat.Hosts.Console", "src/AI.Chat.Hosts.Console"]
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
ENTRYPOINT ["/usr/local/bin/AI.Chat.Hosts.Console"]
