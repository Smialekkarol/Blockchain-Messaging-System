# General information
By default all components are turned off, they can be enabled using following cmake switches:
-DBUILD_ALL_COMPONENTS=ON # build all components
-DBUILD_NODE=ON # enable node build
-DBUILD_GATEWAY=ON # enable gateway build
-DBUILD_CLIENT=ON # enable client build

In order to enable UNIT TESTS add: -DENABLE_UNIT_TESTS=ON

### Build node only
- cmake .. -DBUILD_NODE=ON

### Build node and gateway
- cmake .. -DBUILD_NODE=ON -DBUILD_GATEWAY=ON

### Build all
- cmake .. -DBUILD_NODE=ON -DBUILD_GATEWAY=ON -DBUILD_CLIENT=ON

### Build node with UT
cmake -S . -B build -DBUILD_ALL_COMPONENTS=ON -DENABLE_UNIT_TESTS=ON
cmake --build build -j $(nproc)
