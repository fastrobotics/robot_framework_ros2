#include "Windows/MessageWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    std::string MessageWindow::pretty() {
        std::string str = "---Header Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    bool MessageWindow::new_MessageTextList(std::vector<MessageText> messages) {
        if (messages.size() == 0) {
            // Don't do anything
            return true;
        } else if (messages.size() == 1) {
            setMessageText(messages.at(0).text, messages.at(0).level);
            return true;
        } else {
            fast::rf::Logger::logError("Messages of size: " + std::to_string(messages.size()) + " Not Supported Yet!");
            return false;
        }
    }
    bool MessageWindow::update(double currentTimeSec) {
        auto prevTimeSec = getCurrentTimeSec();
        bool status = BaseWindow::update(currentTimeSec);
        if (status == false) {
            return false;
        }
        double dt = getCurrentTimeSec() - prevTimeSec;
        if (dt > 0.0) {
            m_timerShowingMessageInWindowSec += dt;
        }
        status = updateWindow();
        return status;
    }
    bool MessageWindow::updateWindow() {
        if (getWindow() == nullptr) {
            return false;
        }
        // GCOVR_EXCL_START
        if (m_timerShowingMessageInWindowSec > TIME_TO_SHOW_MESSAGES) {
            m_messageText = "";
            m_messageTextColor = Color::NO_COLOR;
        }
        wattron(getWindow(), COLOR_PAIR(m_messageTextColor));
        mvwprintw(getWindow(), 1, 1, "%s", m_messageText.c_str());
        wclrtoeol(getWindow());
        wattroff(getWindow(), COLOR_PAIR(m_messageTextColor));
        box(getWindow(), 0, 0);
        touchwin(getWindow());
        wrefresh(getWindow());
        return true;
        // GCOVR_EXCL_STOP
    }
    void MessageWindow::setMessageText(std::string text, fast::rf::Level level) {
        if (text == "") {
            return;
        }
        Color color;
        switch (level) {
            case fast::rf::Level::DEBUG:
                color = Color::NO_COLOR;
                break;
            case fast::rf::Level::INFO:
                color = Color::NO_COLOR;
                break;
            case fast::rf::Level::NOTICE:
                color = Color::GREEN_COLOR;
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
        setMessageText(text, color);
    }
    void MessageWindow::setMessageText(std::string text, Color color) {
        m_messageText = text;
        m_messageTextColor = color;
        m_timerShowingMessageInWindowSec = 0.0;
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor