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
COPY ["third_party", "third_party"]
COPY ["CMakeLists.txt", "CMakeLists.txt"]
RUN apk update \
    && apk add g++ \
    && apk add linux-headers \
    && apk add make \
    && apk add cmake \
    && apk add perl

FROM prepare AS configure
ARG BUILD_CONFIGURATION=Release
RUN --mount=type=cache,id=build,target=build \
    --mount=type=cache,id=install,target=install \
    && cmake -B build -S . \
        -DCMAKE_BUILD_TYPE=$BUILD_CONFIGURATION \
        -DCMAKE_INSTALL_PREFIX=install

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
    mkdir deploy/bin \
    && cp install/bin/ai-chat-hosts-console deploy/bin

FROM base AS host
COPY --from=deploy /root/deploy /usr/local
ENTRYPOINT ["/usr/local/bin/ai-chat-hosts-console"]
