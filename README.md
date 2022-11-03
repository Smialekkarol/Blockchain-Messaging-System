By default all components are turned on, they can be disabled using following cmake switches:
-DBUILD_NODE_=OFF # disable node build
-DBUILD_GATEWAY=OFF # disable gateway build
-DBUILD_CLIENT=OFF # disable client build

pre-conditions:
- mkdir build
- cd build

### Build node only
- cmake .. -DBUILD_NODE=ON

### Build node and gateway
- cmake .. -DBUILD_NODE=ON -DBUILD_GATEWAY=ON

### Build all
- cmake .. -DBUILD_NODE=ON -DBUILD_GATEWAY=ON -DBUILD_CLIENT=ON