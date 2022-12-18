How to start client?
in build/ ./src/user ip_address port serverName

example:

./src/user 127.0.0.1 8080 NodeA

# to start rabbitmq-server in the background
service rabbitmq-server start

# to start x service (needed for gui)
service dbus start && service lightdm start