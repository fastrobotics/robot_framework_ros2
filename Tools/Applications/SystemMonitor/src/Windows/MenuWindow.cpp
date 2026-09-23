#include "Windows/MenuWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    std::string MenuWindow::pretty() {
        std::string str = "---Status Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    bool MenuWindow::update(double currentTimeSec) {
        bool status = BaseWindow::update(currentTimeSec);
        if (status == false) {
            return false;
        }
        status = updateWindow();
        return status;
    }
    bool MenuWindow::updateWindow() {
        if (getWindow() == nullptr) {
            return false;
        }
        // GCOVR_EXCL_START
        std::vector<std::string> instructionString;
        // Instructions that are always supported
        // instructionString.push_back("Esc: Reset Screen.  SPACE: Arm/Disarm.");
        // instructionString.push_back("S: Start System Snapshot. (C: Clear Snapshots)");
        // if (diagnostic_mode == DiagnosticMode::SYSTEM) {
        //    instructionString.push_back("D: View NODE Diagnostics");
        //} else if (diagnostic_mode == DiagnosticMode::NODE) {
        //    instructionString.push_back("D: View SYSTEM Diagnostics");
        //}
        if (m_instructionMode == InstructionMode::NODE) {
            instructionString.push_back("L: Change Log Level.");
            // instructionString.push_back("N: Change Node State (1-9).");
        }
        // GCOVR_EXCL_START
        for (std::size_t i = 0; i < instructionString.size(); ++i) {
            mvwprintw(getWindow(), i + 3, 1, "%s", instructionString.at(i).c_str());
            wclrtoeol(getWindow());
        }
        wclrtobot(getWindow());
        box(getWindow(), 0, 0);
        touchwin(getWindow());
        wrefresh(getWindow());
        return true;
        // GCOVR_EXCL_STOP
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor