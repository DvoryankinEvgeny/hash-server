FROM ubuntu:20.04 as build
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get -y install gcc g++ cmake libssl-dev git
COPY . /workspace
WORKDIR /workspace
RUN mkdir /workspace/build && cmake -S . -B build -D CMAKE_BUILD_TYPE=Release && cmake --build build

FROM ubuntu:20.04 as runtime
RUN apt-get update && apt-get -y install libssl-dev && mkdir /workspace

FROM runtime as integration-tests
COPY --from=build /workspace/build/tests/integration_tests/hash-server-integration-tests /workspace
ENV SERVER_ADDRESS "127.0.0.1"
ENV SERVER_PORT "12345"
CMD /workspace/hash-server-integration-tests "${SERVER_ADDRESS}" "${SERVER_PORT}"

FROM runtime as unit-tests
COPY --from=build /workspace/build/tests/unit_tests/hash-server-unit-tests /workspace
COPY --from=build /workspace/build/app/hash-server /workspace
RUN /workspace/hash-server-unit-tests

FROM unit-tests as release
ENV LISTEN_ADDRESS "0.0.0.0"
ENV PORT_NUMBER 12345
EXPOSE $PORT_NUMBER
CMD "/workspace/hash-server" "${LISTEN_ADDRESS}" "${PORT_NUMBER}"
