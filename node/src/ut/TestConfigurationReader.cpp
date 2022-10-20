#include <gtest/gtest.h>
#include <memory>

#include "ConfigurationReader.cpp"

class TestConfigurationReader : public ::testing::Test {
public:
  void SetUp() { sut = std::make_shared<ConfigurationReader>(); }

  std::shared_ptr<ConfigurationReader> sut;
};

TEST_F(TestConfigurationReader, testReadValidConfiguration) {
  const auto &config{std::string(RESOURCES_ROOT) + "validConfig.yaml"};
  auto parsedConfig = sut->read(config);

  ASSERT_TRUE(parsedConfig.has_value());

  auto parsedConfigValue = parsedConfig.value();
  ASSERT_EQ(parsedConfigValue.nodes.size(), 2);

  const auto &firstNode{parsedConfigValue.nodes[0]};
  EXPECT_TRUE(firstNode.name == "NodeB");
  EXPECT_TRUE(firstNode.address == "amqp://localhost/");

  const auto &secondNode{parsedConfigValue.nodes[1]};
  EXPECT_TRUE(secondNode.name == "NodeC");
  EXPECT_TRUE(secondNode.address == "amqp://localhost/");
}

TEST_F(TestConfigurationReader, testReadMissingEntry) {
  const auto &config{std::string(RESOURCES_ROOT) + "missingEntry.yaml"};
  auto parsedConfig = sut->read(config);

  EXPECT_FALSE(parsedConfig.has_value());
}