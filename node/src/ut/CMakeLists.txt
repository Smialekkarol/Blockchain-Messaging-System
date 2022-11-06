file(COPY mocks/missingEntry.yaml DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/mocks")
file(COPY mocks/validConfig.yaml DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/mocks")

add_ut(TestConfigurationReader
  SRC 
    TestConfigurationReader.cpp
    "${CMAKE_CURRENT_LIST_DIR}/../ConfigurationReader.cpp"
  LIB 
    ${YAML_CPP_LIBRARIES} 
    spdlog::spdlog 
    common
)
