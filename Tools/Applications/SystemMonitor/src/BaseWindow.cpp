#include "BaseWindow.hpp"

#include <Infrastructure/Logger.hpp>
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    bool BaseWindow::isInitialized() {
        if (m_name == "") {
            fast::rf::Logger::logError("Window Name is Not Set!");
            return false;
        }
        if (m_win == nullptr) {
            fast::rf::Logger::logError("Window: " + m_name + " Pointer is Null!");
            return false;
        }
        return true;
    }
    void BaseWindow::decrementSelectedRecord() {
        auto _current_record = m_recordSelected;
        _current_record--;
        if (_current_record < 0) {
            _current_record = 0;
        }
        m_recordSelected = _current_record;
    }
    void BaseWindow::incrementSelectedRecord() {
        auto currentRecord = m_recordSelected;
        currentRecord++;
        if (currentRecord >= (m_recordCount - 1)) {
            currentRecord = (m_recordCount - 1);
        }
        m_recordSelected = currentRecord;
    }
    void BaseWindow::updateRecordCount(uint16_t count) { m_recordCount = count; }
    bool BaseWindow::update(double currentTimeSec) {
        m_currentTimeSec = currentTimeSec;
        return true;
    }
    std::string BaseWindow::pretty() {
        if (isInitialized() == false) {
            return m_name + " Is Uninitialized.";
        } else {
            char tempstr[128];
            sprintf(tempstr, "%s (X:%d%%Y:%d%%W:%d%%H:%d%%)", m_name.c_str(), (uint16_t)m_screenCoordPerc.startXPerc,
                    (uint16_t)m_screenCoordPerc.startYPerc, (uint16_t)m_screenCoordPerc.widthPerc,
                    (uint16_t)m_screenCoordPerc.heightPerc);
            return std::string(tempstr);
        }
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor