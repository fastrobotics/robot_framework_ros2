#include "Windows/HeaderWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    void HeaderWindow::newArmCommandMsg(robot_framework_ros2::msg::ArmCommand msg) {
        m_latestArmCommand = fast::rf_ros2::utils::TranslateUtility::convert(msg);
    }
    std::string HeaderWindow::pretty() {
        std::string str = "---Header Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    bool HeaderWindow::update(double currentTimeSec) {
        bool status = BaseWindow::update(currentTimeSec);
        if (status == false) {
            return false;
        }
        status = updateWindow();
        return status;
    }
    bool HeaderWindow::updateWindow() {
        if (getWindow() == nullptr) {
            return false;
        }
        //  GCOVR_EXCL_START
        {  // Time
            std::string str = "Time: " + std::to_string(getCurrentTimeSec());
            str.insert(str.end(), 40 - str.size(), ' ');
            Color color;
            color = Color::WHITE_COLOR;

            wattron(getWindow(), COLOR_PAIR(color));
            mvwprintw(getWindow(), 1, 1, "%s", str.c_str());
            wclrtoeol(getWindow());
            wattroff(getWindow(), COLOR_PAIR(color));
        }
        {  // Armed State

            Color color;
            std::string str = "Armed State: " + fast::rf::pretty(m_latestArmCommand.armed_state);
            str.insert(str.end(), 40 - str.size(), ' ');

            switch (m_latestArmCommand.armed_state) {
                case fast::rf::ArmedState::ARMED:
                    color = Color::BLUE_COLOR;
                    break;  // Should be BLUE for RC Mode, GREEN for Manual, PURPLE for Auto
                case fast::rf::ArmedState::DISARMED_CANNOTARM:
                    color = Color::RED_COLOR;
                    break;
                case fast::rf::ArmedState::DISARMED:
                    color = Color::GREEN_COLOR;
                    break;
                case fast::rf::ArmedState::ARMING:
                    color = Color::GREEN_COLOR;
                    break;
                default:
                    color = Color::RED_COLOR;
                    break;
            }

            wattron(getWindow(), COLOR_PAIR(color));
            mvwprintw(getWindow(), 2, 1, "%s", str.c_str());
            wclrtoeol(getWindow());
            wattroff(getWindow(), COLOR_PAIR(color));
        }

        box(getWindow(), 0, 0);
        wrefresh(getWindow());
        return true;
        // GCOVR_EXCL_STOP
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor