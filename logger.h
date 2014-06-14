#ifndef _LOGGER_H
#define _LOGGER_H

// A single Logger instance can be shared among several threads.
// Forgive me gajop for using variadic templates, I couldn't help myself, the
// evil spirits of lookshinynewthing took over me and made me write this code.

#include <QCoreApplication>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "get_filebuf.h"

class Logger {
public:
    typedef boost::lock_guard<boost::mutex> guard;

    Logger() : eventReceiver(NULL), debugEnabled(false) {}

    void setEventReceiver(QObject* obj) {
        guard lock(m_mutex);
        eventReceiver = obj;
    }
    void setLogFile(const boost::filesystem::path& path) {
        guard lock(m_mutex);
        fileStream.open(path, std::ios::app);
    }
    void setDebug(bool enable) {
        debugEnabled = enable;
    }

    enum class level {
        debug,
        info,
        warning,
        error
    };

    template<typename... Args>
    void debug(Args... args) { putLevel(level::debug, args...); }
    template<typename... Args>
    void info(Args... args) { putLevel(level::info, args...); }
    template<typename... Args>
    void warning(Args... args) { putLevel(level::warning, args...); }
    template<typename... Args>
    void error(Args... args) { putLevel(level::error, args...); }

    struct LogEvent : QEvent {
        LogEvent(level lev, std::string msg) : QEvent(QEvent::Type(TypeId)), lev(lev), msg(msg) {}
        level lev;
        std::string msg;
        static const int TypeId = QEvent::User + 2; // magic numbers ftw!
    };
private:
    template<typename... Args>
    void putLevel(level lev, Args... args) {
        if (lev == level::debug && !debugEnabled)
            return;

        guard lock(m_mutex);

        std::ostringstream msgString;
        std::string tag;
        if (lev == level::debug)
            tag = "[DEBUG]";
        else if (lev == level::info)
            tag = "[INFO]";
        else if (lev == level::warning)
            tag = "[WARNING]";
        else if (lev == level::error)
            tag = "[ERROR]";
        std::cout << std::left << std::setw(9) << tag << " ";
        if (fileStream.good()) {
            time_t t = std::time(NULL);
            std::tm* tm = std::localtime(&t);
            char buf[128];
            std::strftime(buf, 128, "%Y-%m-%d %H:%M:%S", tm);
            fileStream << std::left << std::setw(9) << tag << " [" << buf << "] ";
        }
        put(msgString, args...);

        std::cout << std::endl;
        if (fileStream.good())
            fileStream << std::endl;
        if (eventReceiver)
            QCoreApplication::postEvent(eventReceiver, new LogEvent(lev, msgString.str()));
    }
    void put(std::ostringstream&) {}
    template<typename T, typename... Args>
    void put(std::ostringstream& ss, T val, Args... args) {
        std::cout << val;
        ss << val;
        if (fileStream.good())
            fileStream << val;
        put(ss, args...);
    }

    boost::mutex m_mutex;
    QObject* eventReceiver;
    uofstream fileStream;
    bool debugEnabled;
};

#endif // _LOGGER_H
