FROM ubuntu AS base
USER $APP_UID
WORKDIR ~/

FROM gcc AS prepare
WORKDIR ~/
COPY ["CMakeLists.txt", "CMakeLists.txt"]
COPY ["libs/CMakeLists.txt", "libs/CMakeLists.txt"]
COPY ["libs/boost", "libs/boost"]
COPY ["libs/openssl", "libs/openssl"]
COPY ["src/eboost", "src/eboost"]
COPY ["src/OpenAI", "src/OpenAI"]
COPY ["src/Twitch", "src/Twitch"]
COPY ["src/AI.Chat.Hosts.Console", "src/AI.Chat.Hosts.Console"]
RUN apt-get update && apt-get install -y cmake

FROM prepare AS configure-libs
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake -S libs -B build/libs --config $BUILD_CONFIGURATION

FROM configure-libs AS install-openssl
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake --build build/libs --target install-openssl --config $BUILD_CONFIGURATION

FROM install-openssl AS configure
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake -S . -B build --config $BUILD_CONFIGURATION

FROM configure AS build
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake --build build --target AI.Chat.Hosts.Console --config $BUILD_CONFIGURATION

FROM build AS install
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake --install build --config $BUILD_CONFIGURATION --prefix ~/install

FROM base AS host
COPY --from=install /root/install /usr/local
ENTRYPOINT ["/usr/local/bin/AI.Chat.Hosts.Console"]
