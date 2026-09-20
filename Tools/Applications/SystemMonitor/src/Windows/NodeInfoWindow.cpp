#include <Windows/NodeInfoWindow.hpp>

namespace fast::rf_ros::Tools::Applications::SystemMonitor {
    std::string NodeInfoWindow::pretty() {
        std::string str = "---Node Info Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    void NodeInfoWindow::newHeartbeatMsg(robot_framework_ros2::msg::Heartbeat msg) {
        auto it = nodes.find(msg.NodeName);
        if (it != nodes.end()) {
            it->second.host_device = msg.HostName;
            it->second.base_node_name = msg.BaseNodeName;
            it->second.state = msg.NodeState;
            it->second.last_heartbeat_delta = 0.0;
            it->second.last_heartbeat = getCurrentTimeSec();

        } else {
            insertNode(NodeType::FAST, msg.HostName, msg.BaseNodeName, msg.NodeName);
        }
    }
    void NodeInfoWindow::newReadyToArmMsg(robot_framework_ros2::msg::ReadyToArm msg) {
        auto it = nodes.find(msg.NodeName);
        if (it != nodes.end()) {
            if ((msg.SystemID == 0) || (msg.SubsystemID == 0) || (msg.ProcessID == 0)) {
                it->second.ready_to_arm = "INVALID";
            } else if (msg.ready_to_arm == true) {
                it->second.ready_to_arm = "TRUE";
            } else {
                it->second.ready_to_arm = "FALSE";
            }
            it->second.last_heartbeat_delta = 0.0;
            it->second.last_heartbeat = getCurrentTimeSec();

        } else {
            insertNode(NodeType::FAST, "", "", msg.NodeName);
        }
    }
    bool NodeInfoWindow::insertNode(NodeType node_type, std::string device, std::string base_node_name,
                                    std::string node_name) {
        std::lock_guard<std::mutex> guard(node_list_mutex);
        std::size_t before = nodes.size();
        NodeData newNode(nodes.size(), node_type, device, base_node_name, node_name);
        nodes[newNode.node_name] = newNode;
        std::size_t after = nodes.size();
        updateRecordCount((uint16_t)after);
        return after > before;
    }
    std::string NodeInfoWindow::getWindowHeader() {
        std::string str = "";
        std::map<NodeFieldColumn, Field>::iterator it = node_window_fields.begin();
        while (it != node_window_fields.end()) {
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
        for (auto& pair : nodes) {
            pair.second.last_heartbeat_delta = currentTimeSec - pair.second.last_heartbeat;
            if (pair.second.last_heartbeat_delta > COMMTIMEOUT_THRESHOLD) {
                pair.second.state.state = robot_framework_ros::nodestate::STATE_UNKNOWN;
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
        std::vector<std::pair<std::string, NodeData>> sortedNodes(nodes.begin(), nodes.end());
        std::sort(sortedNodes.begin(), sortedNodes.end(), [](const auto& a, const auto& b) {
            return a.second.id < b.second.id;  // Accesses the struct 'id' via pair.second
        });
        uint16_t index = 0;
        for (const auto& pair : sortedNodes) {
            Color color = Color::UNKNOWN;
            switch (pair.second.state.state) {
                case robot_framework_ros::nodestate::STATE_UNKNOWN:
                    color = Color::RED_COLOR;
                    break;
                case robot_framework_ros::nodestate::STATE_INITIALIZING:
                    color = Color::YELLOW_COLOR;
                    break;
                case robot_framework_ros::nodestate::STATE_STARTING:
                    color = Color::YELLOW_COLOR;
                    break;
                case robot_framework_ros::nodestate::STATE_RUNNING:
                    color = Color::BLUE_COLOR;
                    break;
                case robot_framework_ros::nodestate::STATE_PAUSED:
                    color = Color::GREEN_COLOR;
                    break;
                case robot_framework_ros::nodestate::STATE_RESTART:
                    color = Color::YELLOW_COLOR;
                    break;
                case robot_framework_ros::nodestate::STATE_FINISHED:
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
            std::string str = get_node_info(pair.second, index == getSelectedRecord());
            mvwprintw(getWindow(), TASKSTART_COORD_Y + 2 + (int)index, TASKSTART_COORD_X + 1, str.c_str());
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
            }
        }
        previous_key = key;
        output.message = message;
        return output;
    }
    std::string NodeInfoWindow::get_node_info(NodeData node, bool selected) {
        std::string str = "";
        std::size_t width = 0;
        std::map<NodeFieldColumn, Field>::iterator it;
        it = node_window_fields.find(NodeFieldColumn::MARKER);
        if (it != node_window_fields.end()) {
            width = it->second.width;
            for (std::size_t i = 0; i < width; ++i) {
                if (selected == true) {
                    str += "*";
                } else {
                    str += " ";
                }
            }
        }
        it = node_window_fields.find(NodeFieldColumn::ID);
        if (it != node_window_fields.end()) {
            width = it->second.width;
            std::string tempstr = std::to_string(node.id);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = node_window_fields.find(NodeFieldColumn::HOSTNAME);
        if (it != node_window_fields.end()) {
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
        it = node_window_fields.find(NodeFieldColumn::NODENAME);
        if (it != node_window_fields.end()) {
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
        it = node_window_fields.find(NodeFieldColumn::STATUS);
        if (it != node_window_fields.end()) {
            width = it->second.width;
            std::string tempstr = fast::rf_ros::utils::CoreUtility::pretty(node.state);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = node_window_fields.find(NodeFieldColumn::READY_TO_ARM);
        if (it != node_window_fields.end()) {
            std::string tempstr = node.ready_to_arm;
            std::size_t spaces = it->second.width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }

        it = node_window_fields.find(NodeFieldColumn::RESTARTS);
        if (it != node_window_fields.end()) {
            width = it->second.width;
            std::string tempstr = std::to_string(node.restart_count);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = node_window_fields.find(NodeFieldColumn::PID);
        if (it != node_window_fields.end()) {
            width = it->second.width;
            std::string tempstr = std::to_string(node.pid);
            std::size_t spaces = width - tempstr.size();
            if (spaces > 0) {
                tempstr += std::string(spaces, ' ');
            }
            str += tempstr;
        }
        it = node_window_fields.find(NodeFieldColumn::CPU);
        if (it != node_window_fields.end()) {
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
        it = node_window_fields.find(NodeFieldColumn::RAM);
        if (it != node_window_fields.end()) {
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
        it = node_window_fields.find(NodeFieldColumn::RX);
        if (it != node_window_fields.end()) {
            width = it->second.width;
            std::string max_number_str(width - 4, '9');
            double max_num = std::atof(max_number_str.c_str()) + 0.99;
            if (node.last_heartbeat_delta > max_num) {
                node.last_heartbeat_delta = max_num;
            }
            char tempstr[2 * width];
            sprintf(tempstr, "%2.2f", node.last_heartbeat_delta);
            std::string tempstr_str = std::string(tempstr);
            std::size_t spaces = width - tempstr_str.size();
            if (spaces > 0) {
                tempstr_str += std::string(spaces, ' ');
            }
            str += tempstr_str;
        }

        return str;
    }
}  // namespace fast::rf_ros::Tools::Applications::SystemMonitor