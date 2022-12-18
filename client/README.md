# How to build
Change directory to the Blockchain-Private.

rm -rf build &> /dev/null
cmake -S . -B build -DBUILD_CLIENT=ON
cmake --build build -j $(nproc)

# How to run
Change directory to the Blockchain-Private.

cd build/client/src && ./user NodeA Karol

First argument is ID of the Node
second argument is client name

# how to run rabbitmq in the background 
service rabbitmq-server start

# how to run redis server in the background
redis-server --daemonize yes