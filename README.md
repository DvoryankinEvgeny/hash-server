# hash-server
## What is it?
This repository represents my solution for the test assignment that was given to me by Network Optix company.
## The task
The task was to develop a TCP Server for hash sum calculation. The client application (e.g. telnet or netcat) should be able to open a TCP connection and send strings terminated by “\n” (single byte with the value of 10). The Server should calculate a hash (of any type of your choice) and send it back in HEX terminated by “\n” as well.
The client requests should be processed in parallel, to be able to utilize all of the server CPU cores. The server should be memory-effective and send available data back as soon as it’s available. Incoming strings should not be limited by length.
It is allowed to use any library from standard packages of Ubuntu 20. All of the application modules should have unit test coverage. The server and tests should be able to build and run on Ubuntu 20 as well.
The code should be clean, use any coding style of your choice. There should be a README with a good description of how to build and run the server and tests. The use of docker files and/or build scripts is encouraged.

### How to build
To build the server with unit and integration tests on the host machine:
```bash
mkdir build
cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
```
### Run unittests 
Using ctest:
```bash
cd build
ctest -output-on-failure
```
Run unittests executable:
```bash
./build/tests/unit_tests/hash-server-unit-tests
```
or, more preferable way to do the same is to build a docker image:
```bash
docker build -t hash-server .
```
By default, the hash-server will listen on `0.0.0.0:12345`, but the user can pass environment variables `LISTEN_ADDRESS` and `PORT_NUMBER` to modify this behavior.

Run integration tests:
```bash
docker-compose -f docker-compose.integration-tests.yml up -d
docker-compose -f docker-compose.integration-tests.yml logs tests
```
## Some notes about the solution
I added unit tests mostly for the `Hasher` class only because everything else is the network layer and will be tested with integration tests.

For entities responsible for socket polling and hash calculation, I introduced interfaces just to show my approach in case when multiple implementations are needed. Since there are no requirements to support more than one hash function or polling mechanism, this code is a little bit overcomplicated. My personal opinion is that interfaces should be introduced only when you have at least two implementations. Only in this case, it is possible to write a good interface that does not have to be constantly rewritten and modified.
