# How to build
Change directory to the Blockchain-Private.

rm -rf build &> /dev/null
cmake -S . -B build -DBUILD_GATEWAY=ON
cmake --build build -j $(nproc)

# How to run
Change directory to the Blockchain-Private.

./build/node/src/node /var/fpwork/Blockchain-Private/node/examples/config/NodeA/config.yaml

First argument is configuration file. Please see node/examples for more details.
