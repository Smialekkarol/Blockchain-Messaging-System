# How to build
Change directory to the Blockchain-Private.

rm -rf build &> /dev/null
cmake -S . -B build -DBUILD_CLIENT=ON
cmake --build build -j $(nproc)

# How to run
Change directory to the Blockchain-Private.

./build/client/src/user 127.0.0.1 8080 NodeA

First argument is a address of gateway
Second argument is port of gateway
Third argument is ID of the Node
