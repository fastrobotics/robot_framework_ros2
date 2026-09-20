#include <Windows/StatusWindow.hpp>
namespace fast::rf_ros::Tools::Applications::SystemMonitor {
    std::string StatusWindow::pretty() {
        std::string str = "---Status Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    bool StatusWindow::update(double currentTimeSec) {
        bool status = BaseWindow::update(currentTimeSec);
        if (status == false) {
            return false;
        }
        status = updateWindow();
        return status;
    }
    bool StatusWindow::updateWindow() {
        if (getWindow() == nullptr) {
            return false;
        }
        // GCOVR_EXCL_START
        box(getWindow(), 0, 0);
        wrefresh(getWindow());
        return true;
        // GCOVR_EXCL_STOP
    }
}  // namespace fast::rf_ros::Tools::Applications::SystemMonitor