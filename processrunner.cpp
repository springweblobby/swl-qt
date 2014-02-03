#include "lobbyinterface.h"
#include <cstdlib>
#include <QCoreApplication>
#include <boost/process/mitigate.hpp>

namespace process = boost::process;
namespace asio = boost::asio;

void ProcessRunner::run() {
    using namespace boost::process::initializers;
    using namespace boost::iostreams;
    #if defined BOOST_POSIX_API
        process::pipe stdout_pipe = process::create_pipe();
        auto stdout_sink = file_descriptor_sink(stdout_pipe.sink, close_handle);
        auto stdout_pend = std::make_shared<process::pipe_end>(service, stdout_pipe.source);
        process::pipe stderr_pipe = process::create_pipe();
        auto stderr_sink = file_descriptor_sink(stderr_pipe.sink, close_handle);
        auto stderr_pend = std::make_shared<process::pipe_end>(service, stderr_pipe.source);
    #elif defined BOOST_WINDOWS_API
        #error "Not implemented yet. I've heard creating a named pipe in WINAPI is such a hassle."
    #else
        #error "Unknown platform."
    #endif

    std::vector<std::string> env;
    const char* ptr;
    for(auto i : { // <3 c++11
            "MANPATH",
            "DM_CONTROL",
            "SHELL",
            "XDG_MENU_PREFIX",
            "XDG_SESSION_COOKIE",
            "XDM_MANAGED",
            "USER",
            "PRELINK_PATH_MASK",
            "MULTIOSDIRS",
            "SESSION_MANAGER",
            "PAGER",
            "XDG_CONFIG_DIRS",
            "DESKTOP_SESSION",
            "MAIL",
            "PATH",
            "PWD",
            "EDITOR",
            "LANG",
            "QT_GRAPHICSSYSTEM",
            "PS1",
            "HOME",
            "LANGUAGE",
            "LESS",
            "LOGNAME",
            "XDG_DATA_DIRS",
            "DBUS_SESSION_BUS_ADDRESS",
            "LESSOPEN",
            "INFOPATH",
            "WINDOWPATH",
            "PROFILEHOME",
            "DISPLAY",
            }) {
        if((ptr = std::getenv(i)))
            env.push_back(i + std::string("=") + std::string(ptr));
    }

    try {
        auto child = process::execute(
            set_args(args), // TODO: on Windows it needs a std::wstring range type in a Unicode build.
            set_env(env), // This one too.
            bind_stdout(stdout_sink),
            bind_stderr(stderr_sink),
            close_stdin(),
            #ifdef BOOST_POSIX_API
                notify_io_service(service),
            #endif
            hide_console(),
            throw_on_error()
        );
        terminate_func = [child]() { process::terminate(child); };
    } catch(boost::system::system_error e) {
        throw e;
    }

    // gajop look away
    //
    // Just look at the shitloa... multitude of shared_ptrs this uses.
    // How the hell does this even compile.
    // I don't know if I should laugh or cry.
    auto stdoutBuf = std::make_shared<asio::streambuf>();
    auto stderrBuf = std::make_shared<asio::streambuf>();
    auto onRead = std::make_shared<std::function<void(const boost::system::error_code&, std::size_t)> >();
    auto onErrRead = std::make_shared<std::function<void(const boost::system::error_code&, std::size_t)> >();
    *onRead = [=](const boost::system::error_code& ec, std::size_t bytes){
        if(!ec) {
            std::istream is(stdoutBuf.get());
            std::string msg;
            std::getline(is, msg);
            QCoreApplication::postEvent(eventReceiver, new ReadEvent(cmd, msg));
            asio::async_read_until(*stdout_pend, *stdoutBuf, '\n', *onRead);
        }
    };
    *onErrRead = [=](const boost::system::error_code& ec, std::size_t bytes){
        if(!ec) {
            std::istream is(stderrBuf.get());
            std::string msg;
            std::getline(is, msg);
            QCoreApplication::postEvent(eventReceiver, new ReadEvent(cmd, msg));
            asio::async_read_until(*stderr_pend, *stderrBuf, '\n', *onErrRead);
        }
    };
    asio::async_read_until(*stdout_pend, *stdoutBuf, '\n', *onRead);
    asio::async_read_until(*stderr_pend, *stderrBuf, '\n', *onErrRead);

    #if defined BOOST_POSIX_API
        asio::signal_set set(service, SIGCHLD);
        set.async_wait([=](const boost::system::error_code& ec, const int& _){
            QCoreApplication::postEvent(eventReceiver, new TerminateEvent(cmd));
        });
    #elif defined BOOST_WINDOWS_API
        #error "Not implemented yet."
    #else
        #error "Unknown platform."
    #endif

    thread = boost::thread(boost::bind(&ProcessRunner::runService, this));
}

void ProcessRunner::terminate() {
    try {
        terminate_func();
    } catch(boost::system::system_error e) {
        throw e;
    }
}

ProcessRunner::~ProcessRunner() {
    if(thread.joinable())
        thread.join();
}

ProcessRunner::ProcessRunner(QObject* eventReceiver, const std::string& cmd,
        const std::vector<std::string>& args) : eventReceiver(eventReceiver), cmd(cmd), args(args) {
}

ProcessRunner::ProcessRunner(ProcessRunner&& p) : eventReceiver(p.eventReceiver), cmd(p.cmd), args(p.args),
        terminate_func(p.terminate_func) {
    p.terminate_func = [](){};
    thread = std::move(p.thread);
}

void ProcessRunner::runService() {
    service.run();
}
