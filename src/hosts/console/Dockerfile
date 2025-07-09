FROM ubuntu AS base
USER $APP_UID
WORKDIR /home/$APP_UID

FROM gcc AS prepare
WORKDIR /root
COPY ["src/adapters", "src/adapters"]
COPY ["src/binders", "src/binders"]
COPY ["src/clients", "src/clients"]
COPY ["src/commands", "src/commands"]
COPY ["src/histories", "src/histories"]
COPY ["src/hosts/console", "src/hosts/console"]
COPY ["src/moderators", "src/moderators"]
COPY ["third_party", "third_party"]
COPY ["CMakeLists.txt", "CMakeLists.txt"]
RUN apt-get update && apt-get install -y cmake

FROM prepare AS configure
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake -B build -S . \
        -DCMAKE_BUILD_TYPE=$BUILD_CONFIGURATION

FROM configure AS build
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake --build build --config $BUILD_CONFIGURATION

FROM build AS install
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    cmake --install build --config $BUILD_CONFIGURATION --prefix install

FROM base AS host
COPY --from=install /root/install /usr/local
ENTRYPOINT ["/usr/local/bin/ai-chat-hosts-console"]
