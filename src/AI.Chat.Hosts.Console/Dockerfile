FROM ubuntu AS base
USER $APP_UID
WORKDIR /app

FROM gcc AS restore
RUN apt-get update && apt-get install -y cmake libssl-dev

FROM restore AS init
COPY ["CMakeLists.txt", "/CMakeLists.txt"]
COPY ["src", "/src"]
COPY ["libs", "/libs"]
WORKDIR /build
RUN cmake ../

FROM init AS build
ARG BUILD_CONFIGURATION=Release
WORKDIR /build
RUN cmake --build . --config $BUILD_CONFIGURATION

FROM build AS install
ARG BUILD_CONFIGURATION=Release
RUN cmake --install . --config $BUILD_CONFIGURATION --prefix /install

FROM base AS host
COPY --from=install /install .
ENTRYPOINT ["bin/AI.Chat.Hosts.Console"]
