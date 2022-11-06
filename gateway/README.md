# How to build
Change directory to the Blockchain-Private.

rm -rf build &> /dev/null
cmake -S . -B build -DBUILD_GATEWAY=ON
cmake --build build -j $(nproc)

# How to run
Change directory to the Blockchain-Private.

./build/gateway/src/gateway 127.0.0.1 8080

First argument is address on which gateway is listening
Second argument is port on which gateway is listening
