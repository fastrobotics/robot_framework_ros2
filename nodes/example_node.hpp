/**
 * @file example_node.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "base_node.hpp"
namespace fast::rf_ros2 {
    class ExampleNode : public BaseNode {
       public:
        ExampleNode() : BaseNode("example_node") {}

       protected:
        void run10Hz() override;
        void run1Hz() override;
    };
}  // namespace fast::rf_ros2