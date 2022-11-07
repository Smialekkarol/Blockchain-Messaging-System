target_include_directories(node PRIVATE .)

list(APPEND ${NODE_SOURCES} RedisDB.cpp)
