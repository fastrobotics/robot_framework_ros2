#include "example_node.hpp"
namespace fast::rf_ros2 {
    void ExampleNode::run1Hz() { fast::rf::Logger::logWarn("warn"); }
    void ExampleNode::run10Hz() { fast::rf::Logger::logNotice("notice"); }

    std::shared_ptr<BaseNode> BaseNode::createNode() { return std::make_shared<ExampleNode>(); }
}  // namespace fast::rf_ros2
