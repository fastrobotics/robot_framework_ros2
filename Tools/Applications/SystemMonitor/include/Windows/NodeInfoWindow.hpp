/**
 * @file NodeInfoWindow.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-08-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include <map>
#include <mutex>

#include "BaseWindow.hpp"
#include "robot_framework_ros2/msg/node_state.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {

    /**
     * @brief A Header Window
     *
     */
    class NodeInfoWindow : public BaseWindow {
       public:
        static constexpr double START_X_PERC =
            0.0; /*!< What percentage of the screen to put top left corner (X) of window. */
        static constexpr double START_Y_PERC =
            15.0; /*!< What percentage of the screen to put top left corner (Y) of window. */
        static constexpr double WIDTH_PERC = 66.0;  /*!< What percentage of the screen (Width) to draw the window. */
        static constexpr double HEIGHT_PERC = 55.0; /*!< What percentage of the screen (Height) to draw the window. */
        enum class NodeType { UNKNOWN = 0, FAST = 1, NON_FAST = 2 };
        enum class NodeFieldColumn {
            MARKER = 0,
            ID = 1,
            HOSTNAME = 2,
            NODENAME = 3,
            STATUS = 4,
            READY_TO_ARM = 5,
            RESTARTS = 6,
            PID = 7,
            CPU = 8,
            RAM = 9,
            RX = 10
        };
        /**
         * @brief Construct a new Header Window object
         *
         * @param tabOrder
         * @param mainWindowHeight
         * @param mainWindowWidth
         */
        NodeInfoWindow(std::shared_ptr<rclcpp::Node> node, int16_t tabOrder, int16_t mainWindowHeight,
                       uint16_t mainWindowWidth)
            : BaseWindow(node, "node_info_window", tabOrder, START_X_PERC, START_Y_PERC, WIDTH_PERC, HEIGHT_PERC,
                         mainWindowHeight, mainWindowWidth) {
            ScreenCoordinatePixel coordPix =
                convertCoordinate(getScreenCoordinatesPerc(), mainWindowWidth, mainWindowHeight);
            supportedKeys.push_back(KEY_UP);
            supportedKeys.push_back(KEY_DOWN);
            supportedKeys.push_back('l');
            supportedKeys.push_back('L');
            supportedKeys.push_back('0');
            supportedKeys.push_back('1');
            supportedKeys.push_back('2');
            supportedKeys.push_back('3');
            supportedKeys.push_back('4');
            supportedKeys.push_back('5');
            supportedKeys.push_back('6');
            supportedKeys.push_back('7');
            supportedKeys.push_back('8');
            supportedKeys.push_back('9');
            m_nodeWindowFields.insert(std::pair<NodeFieldColumn, Field>(NodeFieldColumn::MARKER, Field("", 3)));
            m_nodeWindowFields.insert(std::pair<NodeFieldColumn, Field>(NodeFieldColumn::ID, Field("ID", 4)));
            // m_nodeWindowFields.insert(
            //     std::pair<NodeFieldColumn, Field>(NodeFieldColumn::HOSTNAME, Field(" Host ", 20)));
            m_nodeWindowFields.insert(
                std::pair<NodeFieldColumn, Field>(NodeFieldColumn::NODENAME, Field(" NodeName ", 70)));
            m_nodeWindowFields.insert(
                std::pair<NodeFieldColumn, Field>(NodeFieldColumn::STATUS, Field(" Status ", 14)));
            m_nodeWindowFields.insert(
                std::pair<NodeFieldColumn, Field>(NodeFieldColumn::READY_TO_ARM, Field(" Ready To Arm ", 15)));
            // m_nodeWindowFields.insert(
            //    std::pair<NodeFieldColumn, Field>(NodeFieldColumn::RESTARTS, Field(" Restarts ", 10)));
            // m_nodeWindowFields.insert(std::pair<NodeFieldColumn, Field>(NodeFieldColumn::PID, Field(" PID ", 8)));
            // m_nodeWindowFields.insert(std::pair<NodeFieldColumn, Field>(NodeFieldColumn::CPU, Field(" CPU(%) ",
            // 10))); m_nodeWindowFields.insert(std::pair<NodeFieldColumn, Field>(NodeFieldColumn::RAM, Field(" RAM(%)",
            // 10)));
            m_nodeWindowFields.insert(std::pair<NodeFieldColumn, Field>(NodeFieldColumn::RX, Field(" Rx ", 6)));
            WINDOW* win = createNewWin(coordPix.heightPix, coordPix.widthPix, coordPix.startYPix, coordPix.startXPix);
            setScreenCoordinatesPix(coordPix);
            setWindow(win);

            std::string header = getWindowHeader();
            mvwprintw(win, 1, 1, "%s", header.c_str());
            std::string dashed(getScreenCoordinatesPixel().widthPix - 2, '-');
            mvwprintw(win, 2, 1, "%s", dashed.c_str());
            wrefresh(win);
        }
        KeyEventContainer newKeyEvent(int key) override;
        void newArmCommandMsg([[maybe_unused]] robot_framework_ros2::msg::ArmCommand msg) override {}

        void newHeartbeatMsg(robot_framework_ros2::msg::Heartbeat msg) override;

        void newReadyToArmMsg(robot_framework_ros2::msg::ReadyToArm msg) override;

        void newDiagnosticMsg([[maybe_unused]] robot_framework_ros2::msg::Diagnostic msg) override {}

        std::string get_m_selectedNode() { return m_selectedNode; }

        /**
         * @brief Human readable string
         *
         * @return std::string
         */
        std::string pretty() override;

        /**
         * @brief Update at a periodic rate
         *
         * @param currentTimeSec
         * @return true
         * @return false
         */
        bool update(double currentTimeSec) override;

       protected:
        bool updateWindow();

       private:
        struct NodeData {
            NodeData() = default;
            NodeData(int16_t _id, NodeType _type, std::string _host_device, std::string _base_node_name,
                     std::string _node_name)
                : id(_id),
                  type(_type),
                  pid(0),
                  host_device(_host_device),
                  base_node_name(_base_node_name),
                  node_name(_node_name),
                  cpu_used_perc(0.0),
                  last_heartbeat(0.0),
                  last_heartbeat_delta(0.0),
                  restart_count(0) {
                state.state = robot_framework_ros2::msg::NodeState::STATE_STARTING;
            }
            bool initialized;
            uint16_t id;
            robot_framework_ros2::msg::NodeState state;
            NodeType type;
            uint16_t pid;
            std::string host_device;
            std::string base_node_name;
            std::string node_name;
            std::string ready_to_arm{"UNKNOWN"};
            double cpu_used_perc;
            double mem_used_perc;
            double last_heartbeat;
            double last_heartbeat_delta;
            uint64_t restart_count;
        };
        bool insertNode(NodeType node_type, std::string device, std::string base_node_name, std::string node_name);
        std::string getWindowHeader();
        std::string getNodeInfo(NodeData node, bool selected);

        int m_previousKey{-1};
        std::mutex m_nodeListMutex;
        std::map<NodeFieldColumn, Field> m_nodeWindowFields;
        std::string m_selectedNode{""};
        std::map<std::string, NodeData> m_nodes;
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor