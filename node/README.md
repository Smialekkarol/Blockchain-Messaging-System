# How to build
Change directory to the Blockchain-Private.

rm -rf build &> /dev/null
cmake -S . -B build -DBUILD_GATEWAY=ON
cmake --build build -j $(nproc)

# How to run
Change directory to the Blockchain-Private.

./build/node/src/node /var/fpwork/Blockchain-Private/node/examples/config/NodeA/config.yaml

First argument is configuration file. Please see node/examples for more details.

# How to create rabbitmq user
Comands to create needed user for each node.
rabbitmqctl add_user user1 user1
rabbitmqctl set_user_tags user1 administrator
rabbitmqctl set_permissions -p / user1 ".*" ".*" ".*"


# how to run rabbitmq in the background 
service rabbitmq-server start

# how to run redis server in the background
redis-server --daemonize yes
