#include "Windows/NodeInfoWindow.hpp"

#include "robot_framework_ros2/srv/change_logger_level.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    std::string NodeInfoWindow::pretty() {
        std::string str = "---Node Info Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    void NodeInfoWindow::newHeartbeatMsg(robot_framework_ros2::msg::Heartbeat msg) {
        auto it = m_nodes.find(msg.nodename);
        if (it != m_nodes.end()) {
            it->second.host_device = msg.hostname;
            it->second.base_node_name = msg.base_nodename;
            it->second.state = msg.node_state;
            it->second.last_heartbeat_delta = 0.0;
            it->second.last_heartbeat = getCurrentTimeSec();

        } else {
            insertNode(NodeType::FAST, msg.hostname, msg.base_nodename, msg.nodename);
        }
    }
    void NodeInfoWindow::newReadyToArmMsg(robot_framework_ros2::msg::ReadyToArm msg) {
        auto it = m_nodes.find(msg.nodename);
        if (it != m_nodes.end()) {
            if ((msg.system_id == 0) || (msg.subsystem_id == 0) || (msg.process_id == 0)) {
                it->second.ready_to_arm = "INVALID";
            } else if (msg.ready_to_arm == true) {
                it->second.ready_to_arm = "TRUE";
            } else {
                it->second.ready_to_arm = "FALSE";
            }
            it->second.last_heartbeat_delta = 0.0;
            it->second.last_heartbeat = getCurrentTimeSec();

        } else {
            insertNode(NodeType::FAST, "", "", msg.nodename);
        }
    }
    bool NodeInfoWindow::insertNode(NodeType node_type, std::string device, std::string base_node_name,
                                    std::string node_name) {
        std::lock_guard<std::mutex> guard(m_nodeListMutex);
        std::size_t before = m_nodes.size();
        NodeData newNode(m_nodes.size(), node_type, device, base_node_name, node_name);
        m_nodes[newNode.node_name] = newNode;
        std::size_t after = m_nodes.size();
        updateRecordCount((uint16_t)after);
        return after > before;
    }
    std::string NodeInfoWindow::getWindowHeader() {
        std::string str = "";
        std::map<NodeFieldColumn, Field>::iterator it = m_nodeWindowFields.begin();
        while (it != m_nodeWindowFields.end()) {
            // Check if field name is too long:
            if (it->second.text.size() > it->second.width) {
                str += it->second.text.substr(0, it->second.width);
            } else {
                str += it->second.text;
                // Figure out how many spaces to add
                std::size_t spaces = it->second.width - it->second.text.size();
                for (std::size_t j = 0; j < spaces; ++j) {
                    str += " ";
                }
            }
            ++it;
        }

        if (str.size() > getMainWindowWidth()) {
            fast::rf::Logger::logWarn("Node Header too long for Window!.");
            return "";
        }
        return str;
    }
    bool NodeInfoWindow::update(double currentTimeSec) {
        bool status = BaseWindow::update(currentTimeSec);
        if (status == false) {
            return false;
        }
        for (auto& pair : m_nodes) {
            pair.second.last_heartbeat_delta = currentTimeSec - pair.second.last_heartbeat;
            if (pair.second.last_heartbeat_delta > COMMTIMEOUT_THRESHOLD) {
                pair.second.state.state = robot_framework_ros2::msg::NodeState::STATE_UNKNOWN;
                pair.second.ready_to_arm = "UNKNOWN";
            }
        }
        status = updateWindow();
        return status;
    }
    bool NodeInfoWindow::updateWindow() {
        if (getWindow() == nullptr) {
            return false;
        }
        // GCOVR_EXCL_START
        const uint16_t TASKSTART_COORD_Y = 1;
        const uint16_t TASKSTART_COORD_X = 1;
        std::vector<std::pair<std::string, NodeData>> sortedNodes(m_nodes.begin(), m_nodes.end());
        std::sort(sortedNodes.begin(), sortedNodes.end(), [](const auto& a, const auto& b) {
            return a.second.id < b.second.id;  // Accesses the struct 'id' via pair.second
        });
        uint16_t index = 0;
        for (const auto& pair : sortedNodes) {
            Color color = Color::UNKNOWN;
            switch (pair.second.state.state) {
                case robot_framework_ros2::msg::NodeState::STATE_UNKNOWN:
                    color = Color::RED_COLOR;
                    break;
                case robot_framework_ros2::msg::NodeState::STATE_INITIALIZING:
                    color = Color::YELLOW_COLOR;
                    break;
                case robot_framework_ros2::msg::NodeState::STATE_STARTING:
                    color = Color::YELLOW_COLOR;
                    break;
                case robot_framework_ros2::msg::NodeState::STATE_RUNNING:
                    color = Color::BLUE_COLOR;
                    break;
                case robot_framework_ros2::msg::NodeState::STATE_PAUSED:
                    color = Color::GREEN_COLOR;
                    break;
                case robot_framework_ros2::msg::NodeState::STATE_RESTART:
                    color = Color::YELLOW_COLOR;
                    break;
                case robot_framework_ros2::msg::NodeState::STATE_FINISHED:
                    color = Color::YELLOW_COLOR;
                    break;
                default:
                    color = Color::RED_COLOR;
                    break;
            }
            if (index == getSelectedRecord()) {
                m_selectedNode = pair.second.node_name;
            }

            wattron(getWindow(), COLOR_PAIR(color));
            std::string str = getNodeInfo(pair.second, index == getSelectedRecord());
            mvwprintw(getWindow(), TASKSTART_COORD_Y + 2 + (int)index, TASKSTART_COORD_X + 1, "%s", str.c_str());
            wclrtoeol(getWindow());
            wattroff(getWindow(), COLOR_PAIR(color));
            index++;
        }
        if (getFocused()) {
            box(getWindow(), '.', '.');
        } else {
            box(getWindow(), 0, 0);
        }

        wrefresh(getWindow());
        return true;
        // GCOVR_EXCL_STOP
    }
    KeyEventContainer NodeInfoWindow::newKeyEvent(int key) {
        KeyEventContainer output;
        MessageText message;
        if (std::find(supportedKeys.begin(), supportedKeys.end(), key) != supportedKeys.end()) {
            output.message.level = fast::rf::Level::ERROR;  // Set default Level to error, so if any supported keys
                                                            // are not processed, will actively fail.
        } else {
            return output;
        }
        if (getFocused() == true) {
            if (key == KEY_UP) {
                decrementSelectedRecord();
            } else if (key == KEY_DOWN) {
                incrementSelectedRecord();
            } else if ((key == 'l') || (key == 'L')) {
                std::string str = "Enter new Log Level ";
                for (uint8_t i = (uint8_t)fast::rf::Level::UNKNOWN; i < (uint8_t)fast::rf::Level::END_OF_LIST; ++i) {
                    if (i == (uint8_t)fast::rf::Level::UNKNOWN) {
                        // Do nothing
                    } else {
                        str += std::to_string(i) + ":" + fast::rf::pretty((fast::rf::Level)i) + " ";
                    }
                }
                message = MessageText(str, fast::rf::Level::NOTICE);
            } else if ((key == '0') || (key == '1') || (key == '2') || (key == '3') || (key == '4') || (key == '5') ||
                       (key == '6') || (key == '7') || (key == '8') || (key == '9')) {
                if (m_nodes.size() == 0) {
                    return output;
                }
                if ((m_previousKey == 'l') || (m_previousKey == 'L')) {
                    uint8_t verbosityValue = key - '0';
                    auto verbosity = fast::rf::pretty((fast::rf::Level)verbosityValue);
                    if (verbosity == "UNKNOWN") {
                        std::string str = "Requested Log Level Not Supported.";
                        message = MessageText(str, fast::rf::Level::WARN);
                    } else {
                        auto selectedNodeInfoIt = m_nodes.find(m_selectedNode);
                        if (selectedNodeInfoIt != m_nodes.end()) {
                        } else {
                            message = MessageText("Can't lookup Node: " + m_selectedNode, fast::rf::Level::WARN);
                        }
                        std::string serviceTopic = m_selectedNode + "/change_logger_level";
                        auto clientChangeLoggerLevel =
                            getNode()->create_client<robot_framework_ros2::srv::ChangeLoggerLevel>(serviceTopic);
                        if (!clientChangeLoggerLevel->wait_for_service(std::chrono::milliseconds(500))) {
                            message =
                                MessageText("Service: " + serviceTopic + " Not available!", fast::rf::Level::WARN);
                        }
                        auto request = std::make_shared<robot_framework_ros2::srv::ChangeLoggerLevel::Request>();
                        request->verbosity_level = verbosityValue;
                        clientChangeLoggerLevel->async_send_request(
                            request,
                            [this, clientChangeLoggerLevel](
                                rclcpp::Client<robot_framework_ros2::srv::ChangeLoggerLevel>::SharedFuture future) {
                                try {
                                    auto response = future.get();
                                    if (response->request_approved == true) {
                                        fast::rf::Logger::logNotice("Logger Change Service Approved.");

                                    } else {
                                        fast::rf::Logger::logError("Logger Change Service Rejected");
                                    }

                                } catch (const std::exception& e) {
                                    fast::rf::Logger::logError("Logger Service Change Failed: " +
                                                               std::string(e.what()));
                                }
                            });
                    }
                }
            }
        }
        m_previousKey = key;
        output.message = message;
        return output;
    }
    std::string NodeInfoWindow::getNodeInfo(NodeData node, bool selected) {
        std::string str = "";
        std::size_t width = 0;
        std::map<NodeFieldColumn, Field>::iterator it;
        it = m_nodeWindowFields.find(NodeFieldColumn::MARKER);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            for (std::size_t i = 0; i < width; ++i) {
                if (selected == true) {
                    str += "*";
                } else {
                    str += " ";
                }
            }
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::ID);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            std::string tempstr = std::to_string(node.id);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::HOSTNAME);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            std::string tempstr = node.host_device;
            if (tempstr.size() > width) {
                tempstr = tempstr.substr(0, width - 4) + "... ";
            } else {
                if (tempstr.size() > (std::size_t)(width - 1)) {
                    tempstr = tempstr.substr(0, (width - 1));
                    tempstr += " ";
                } else {
                    std::size_t spaces = width - tempstr.size();
                    if (spaces > 0) {
                        tempstr += std::string(spaces, ' ');
                    }
                }
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::NODENAME);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            std::string tempstr = node.node_name;
            std::size_t found_hostname = node.node_name.find(node.host_device);
            if (found_hostname != std::string::npos) {
                tempstr.replace(found_hostname, node.host_device.length(), "");
            }
            if (tempstr.size() > width) {
                tempstr = tempstr.substr(0, width - 4) + "... ";
            } else {
                std::size_t spaces = width - tempstr.size();
                if (spaces > 0) {
                    tempstr += std::string(spaces, ' ');
                }
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::STATUS);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            std::string tempstr = fast::rf_ros2::utils::CoreUtility::pretty(node.state);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::READY_TO_ARM);
        if (it != m_nodeWindowFields.end()) {
            std::string tempstr = node.ready_to_arm;
            std::size_t spaces = it->second.width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }

        it = m_nodeWindowFields.find(NodeFieldColumn::RESTARTS);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            std::string tempstr = std::to_string(node.restart_count);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::PID);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            std::string tempstr = std::to_string(node.pid);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::CPU);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            char c_tempstr[8];
            sprintf(c_tempstr, "%3.2f", node.cpu_used_perc);
            std::string tempstr = std::string(c_tempstr);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::RAM);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            char c_tempstr[8];
            sprintf(c_tempstr, "%3.2f", node.mem_used_perc);
            std::string tempstr = std::string(c_tempstr);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = m_nodeWindowFields.find(NodeFieldColumn::RX);
        if (it != m_nodeWindowFields.end()) {
            width = it->second.width;
            std::string max_number_str(width - 4, '9');
            double max_num = std::atof(max_number_str.c_str()) + 0.99;
            if (node.last_heartbeat_delta > max_num) {
                node.last_heartbeat_delta = max_num;
            }
            char tempstr[64];
            std::snprintf(tempstr, sizeof(tempstr), "%2.2f", node.last_heartbeat_delta);
            std::string tempstr_str(tempstr);
            if (width > tempstr_str.size()) {
                tempstr_str.append(width - tempstr_str.size(), ' ');
            }
            str += tempstr_str;
        }

        return str;
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor