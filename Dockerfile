FROM ubuntu:20.04 as build
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get -y install gcc g++ cmake libssl-dev git
COPY . /workspace
WORKDIR /workspace
RUN mkdir /workspace/build && cmake -S . -B build -D CMAKE_BUILD_TYPE=Release && cmake --build build

FROM ubuntu:20.04 as tests
RUN apt-get update && apt-get -y install libssl-dev && mkdir /workspace
COPY --from=build /workspace/build/tests/hash-server-tests /workspace
COPY --from=build /workspace/build/app/hash-server /workspace
RUN /workspace/hash-server-tests

FROM tests as release
ENV LISTEN_ADDRESS="0.0.0.0"
ENV PORT_NUMBER=12345
EXPOSE $PORT_NUMBER
CMD "/workspace/hash-server" "${LISTEN_ADDRESS}" "${PORT_NUMBER}"
