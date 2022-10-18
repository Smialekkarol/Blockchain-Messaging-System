alias start_desktop="service dbus start && service lightdm start";
alias stop_desktop="service dbus stop && service lightdm stop";
alias start_mongodb="mongod --dbpath /var/lib/mongo --logpath /var/log/mongodb/mongod.log --fork";
alias start_redis_server="redis-server";