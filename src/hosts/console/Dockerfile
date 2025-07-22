FROM alpine AS base
USER $APP_UID
WORKDIR /home/$APP_UID

FROM alpine AS prepare
WORKDIR /root
COPY ["src/adapters", "src/adapters"]
COPY ["src/binders", "src/binders"]
COPY ["src/clients", "src/clients"]
COPY ["src/commands", "src/commands"]
COPY ["src/histories", "src/histories"]
COPY ["src/hosts/console", "src/hosts/console"]
COPY ["src/moderators", "src/moderators"]
COPY ["src/telemetry", "src/telemetry"]
COPY ["third_party/abseil", "third_party/abseil"]
COPY ["third_party/boost", "third_party/boost"]
COPY ["third_party/cares", "third_party/cares"]
COPY ["third_party/eboost", "third_party/eboost"]
COPY ["third_party/esqlite", "third_party/esqlite"]
COPY ["third_party/grpc", "third_party/grpc"]
COPY ["third_party/openssl", "third_party/openssl"]
COPY ["third_party/opentelemetry", "third_party/opentelemetry"]
COPY ["third_party/opentelemetry-proto", "third_party/opentelemetry-proto"]
COPY ["third_party/protobuf", "third_party/protobuf"]
COPY ["third_party/re2", "third_party/re2"]
COPY ["third_party/sqlite", "third_party/sqlite"]
COPY ["third_party/zlib", "third_party/zlib"]
COPY ["third_party/CMakeLists.txt", "third_party/CMakeLists.txt"]
COPY ["CMakeLists.txt", "CMakeLists.txt"]
RUN apk update \
    && apk add g++ \
    && apk add linux-headers \
    && apk add make \
    && apk add cmake \
    && apk add perl

FROM prepare AS configure
ARG BUILD_CONFIGURATION=Release
ARG TELEMETRY=ON
RUN --mount=type=cache,id=build,target=build \
    --mount=type=cache,id=install,target=install \
    cmake -B build -S . \
        -DCMAKE_BUILD_TYPE=$BUILD_CONFIGURATION \
        -DCMAKE_INSTALL_PREFIX=install \
        -DTELEMETRY=$TELEMETRY

FROM configure AS build
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    --mount=type=cache,id=install,target=install \
    cmake --build build --config $BUILD_CONFIGURATION

FROM build AS install
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    --mount=type=cache,id=install,target=install \
    cmake --install build --config $BUILD_CONFIGURATION --prefix install

FROM install AS deploy
RUN --mount=type=cache,id=install,target=install \
    mkdir deploy && mkdir deploy/bin \
    && cp install/bin/ai-chat-hosts-console deploy/bin

FROM base AS host
COPY --from=deploy /root/deploy /usr/local
ENTRYPOINT ["/usr/local/bin/ai-chat-hosts-console"]
