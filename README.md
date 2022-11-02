By default all components are turned on, they can be disabled using following cmake switches:
-DBUILD_NODE_=OFF # disable node build
-DBUILD_GATEWAY=OFF # disable gateway build
-DBUILD_CLIENT=OFF # disable client build

pre-conditions:
- mkdir build
- cd build

### Build all
- cmake ..

### Build node only
- cmake .. -DBUILD_GATEWAY=OFF -DBUILD_CLIENT=OFF

### Build node and gateway
- cmake .. -DBUILD_CLIENT=OFF