#include "Windows/DiagnosticWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    std::string DiagnosticWindow::pretty() {
        std::string str = "---Diagnostic Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    void DiagnosticWindow::newDiagnosticMsg(robot_framework_ros2::msg::Diagnostic msg) {
        m_nodeDiagnosticMonitors[msg.node_name][msg.diagnostic_type].nodeDiagnostic = msg;
        m_nodeDiagnosticMonitors[msg.node_name][msg.diagnostic_type].lastUpdateSec = getCurrentTimeSec();
    }
    bool DiagnosticWindow::update(double currentTimeSec) {
        bool status = BaseWindow::update(currentTimeSec);
        if (status == false) {
            return false;
        }
        for (auto& nodeDiagnostics : m_nodeDiagnosticMonitors) {
            for (auto& diagnostic : nodeDiagnostics.second) {
                diagnostic.second.lastUpdateDeltaSec = (getCurrentTimeSec() - diagnostic.second.lastUpdateSec);
                if (diagnostic.second.lastUpdateDeltaSec > COMMTIMEOUT_THRESHOLD) {
                    diagnostic.second.nodeDiagnostic.level = (uint8_t)fast::rf::Level::UNKNOWN;
                }
            }
        }
        status = updateWindow();
        return status;
    }
    std::string DiagnosticWindow::getWindowHeader() {
        if (m_windowMode == DiagnosticWindowMode::NODE) {
            return "  NODE DIAGNOSTICS";
        }
        return "";
    }
    bool DiagnosticWindow::updateWindow() {
        if (getWindow() == nullptr) {
            return false;
        }
        if (m_windowMode == DiagnosticWindowMode::NODE) {
            auto nodeDiagnosticIt = m_nodeDiagnosticMonitors.find(m_nodeToMonitor);
            if (nodeDiagnosticIt == m_nodeDiagnosticMonitors.end()) {
                // Node not present yet.  Don't do anything.
                return true;
            }
            const uint16_t DIAGSTART_COORD_Y = 1;
            const uint16_t DIAGSTART_COORD_X = 1;
            uint16_t index = 0;
            for (auto i = (uint8_t)fast::rf::DiagnosticDefinition::DiagnosticType::UNKNOWN;
                 i < (uint8_t)fast::rf::DiagnosticDefinition::DiagnosticType::END_OF_LIST; ++i) {
                fast::rf::DiagnosticDefinition::DiagnosticType diagnosticType =
                    (fast::rf::DiagnosticDefinition::DiagnosticType)i;
                if ((diagnosticType == fast::rf::DiagnosticDefinition::DiagnosticType::UNKNOWN) ||
                    (diagnosticType == fast::rf::DiagnosticDefinition::DiagnosticType::UNKNOWN_TYPE) ||
                    (diagnosticType == fast::rf::DiagnosticDefinition::DiagnosticType::END_OF_LIST)) {
                    continue;
                }
                auto nodeDiagnosticMap = nodeDiagnosticIt->second;
                auto diagnosticTypeIt = nodeDiagnosticMap.find(i);

                if (diagnosticTypeIt != nodeDiagnosticMap.end()) {
                    fast::rf::Level level = (fast::rf::Level)diagnosticTypeIt->second.nodeDiagnostic.level;
                    fast::rf::DiagnosticDefinition::DiagnosticMessage diag_message =
                        (fast::rf::DiagnosticDefinition::DiagnosticMessage)
                            diagnosticTypeIt->second.nodeDiagnostic.diagnostic_message;
                    Color color = Color::UNKNOWN;
                    switch (level) {
                        case fast::rf::Level::UNKNOWN:
                            color = Color::RED_COLOR;
                            break;
                        case fast::rf::Level::NOERROR:
                            color = Color::BLUE_COLOR;
                            break;
                        case fast::rf::Level::DEBUG:
                            color = Color::BLUE_COLOR;
                            break;
                        case fast::rf::Level::INFO:
                            color = Color::BLUE_COLOR;
                            break;
                        case fast::rf::Level::NOTICE:
                            color = Color::BLUE_COLOR;
                            break;
                        case fast::rf::Level::WARN:
                            color = Color::YELLOW_COLOR;
                            break;
                        case fast::rf::Level::ERROR:
                            color = Color::RED_COLOR;
                            break;
                        case fast::rf::Level::FATAL:
                            color = Color::RED_COLOR;
                            break;
                        default:
                            color = Color::RED_COLOR;
                            break;
                    }

                    wattron(getWindow(), COLOR_PAIR(color));
                    std::string str =
                        fast::rf::DiagnosticDefinition::pretty(diagnosticType) + ":" +
                        fast::rf::DiagnosticDefinition::pretty(diag_message) + "(" +

                        std::to_string(static_cast<double>(diagnosticTypeIt->second.nodeDiagnostic.timestamp.sec) +
                                       static_cast<double>(diagnosticTypeIt->second.nodeDiagnostic.timestamp.nanosec) *
                                           1e-9) +
                        ") - " + diagnosticTypeIt->second.nodeDiagnostic.description;
                    int windowWidth = getmaxx(getWindow());
                    if (str.size() >= (std::size_t)windowWidth) {
                        str.erase(windowWidth - 3);
                    }
                    mvwprintw(getWindow(), DIAGSTART_COORD_Y + 2 + (int)index, DIAGSTART_COORD_X + 1, "%s",
                              str.c_str());
                    wclrtoeol(getWindow());
                    wattroff(getWindow(), COLOR_PAIR(color));

                } else {
                    Color color = Color::NO_COLOR;
                    wattron(getWindow(), COLOR_PAIR(color));
                    std::string str = fast::rf::DiagnosticDefinition::pretty(diagnosticType);
                    mvwprintw(getWindow(), DIAGSTART_COORD_Y + 2 + (int)index, DIAGSTART_COORD_X + 1, "%s",
                              str.c_str());
                    wclrtoeol(getWindow());
                    wattroff(getWindow(), COLOR_PAIR(color));
                }

                index++;
            }
        }

        // GCOVR_EXCL_START
        box(getWindow(), 0, 0);
        wrefresh(getWindow());
        return true;
        // GCOVR_EXCL_STOP
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor