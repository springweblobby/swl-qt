#include "lobbyinterface.h"
#include <cstdlib>
#include <exception>
#include <boost/process/mitigate.hpp>
#include <boost/chrono/include.hpp>
#ifdef BOOST_WINDOWS_API
    #include <windows.h>
    #include <ctime>
    #include <random>
#endif

namespace process = boost::process;
namespace asio = boost::asio;

typedef asio::buffers_iterator<asio::streambuf::const_buffers_type> buf_iterator;
std::pair<buf_iterator, bool> matchNewline(buf_iterator begin, buf_iterator end) {
    for(auto i = begin; i != end; i++) {
        if(*i == '\r' && i + 1 != end && *(++i) == '\n')
            return std::make_pair(++i, true);
        else if(*i == '\n' || *i == '\r')
            return std::make_pair(++i, true);
    }
    return std::make_pair(end, false);
}

void ProcessRunner::run() {
    using namespace boost::process::initializers;
    using namespace boost::iostreams;
    #if defined BOOST_POSIX_API
        process::pipe stdout_pipe = process::create_pipe();
        auto stdout_sink = std::make_shared<file_descriptor_sink>(stdout_pipe.sink, close_handle);
        auto stdout_pend = std::make_shared<process::pipe_end>(service, stdout_pipe.source);

        process::pipe stderr_pipe = process::create_pipe();
        auto stderr_sink = std::make_shared<file_descriptor_sink>(stderr_pipe.sink, close_handle);
        auto stderr_pend = std::make_shared<process::pipe_end>(service, stderr_pipe.source);
    #elif defined BOOST_WINDOWS_API
        static std::minstd_rand rand(std::time(NULL));
        std::wstring pipe_name = L"\\\\.\\pipe\\springweblobby" + std::to_wstring(rand());

        HANDLE stdout_pipe_source = CreateNamedPipe((pipe_name + L"stdout").c_str(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_REJECT_REMOTE_CLIENTS, 1, 1024*32, 1024*32, 0, NULL);
        HANDLE stdout_pipe_sink = CreateFile((pipe_name + L"stdout").c_str(), GENERIC_WRITE, 0, NULL,
            OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        auto stdout_sink = std::make_shared<file_descriptor_sink>(stdout_pipe_sink, never_close_handle);
        auto stdout_pend = std::make_shared<process::pipe_end>(service, stdout_pipe_source);

        HANDLE stderr_pipe_source = CreateNamedPipe((pipe_name + L"stderr").c_str(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_REJECT_REMOTE_CLIENTS, 1, 1024*32, 1024*32, 0, NULL);
        HANDLE stderr_pipe_sink = CreateFile((pipe_name + L"stderr").c_str(), GENERIC_WRITE, 0, NULL,
            OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        auto stderr_sink = std::make_shared<file_descriptor_sink>(stderr_pipe_sink, never_close_handle);
        auto stderr_pend = std::make_shared<process::pipe_end>(service, stderr_pipe_source);
    #endif

    std::vector<std::string> env;
    const char* ptr;
    // The list of env vars contains unnecessary entries, but they don't hurt
    // at all, so I'm not going to clean it.
    for(auto i : { // <3 c++11

            // Posix vars
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
            "LD_LIBRARY_PATH",
            "QT_PLUGIN_PATH",
            "GST_PLUGIN_PATH",
            "OMP_THREAD_LIMIT",
            "OMP_WAIT_POLICY",

            // Windows vars
            "ALLUSERSPROFILE",
            "APPDATA",
            "CommonProgramFiles",
            "COMPUTERNAME",
            "ComSpec",
            "FP_NO_HOST_CHECK",
            "HOMEDRIVE",
            "HOMEPATH",
            "LOCALAPPDATA",
            "LOGONSERVER",
            "NUMBER_OF_PROCESSORS",
            "OS",
            "PATHEXT",
            "PROCESSOR_ARCHITECTURE",
            "PROCESSOR_IDENTIFIER",
            "PROCESSOR_LEVEL",
            "PROCESSOR_REVISION",
            "ProgramData",
            "ProgramFiles",
            "PROMPT",
            "PSModulePath",
            "PUBLIC",
            "SystemDrive",
            "SystemRoot",
            "TEMP",
            "TMP",
            "USERDOMAIN",
            "USERNAME",
            "USERPROFILE",
            "windir",
            "__COMPAT_LAYER",

            }) {
        if((ptr = std::getenv(i)))
            env.push_back(i + std::string("=") + std::string(ptr));
    }

    #ifdef BOOST_WINDOWS_API
        std::vector<std::wstring> wenv;
        for(auto i : env)
            wenv.push_back(std::wstring(i.begin(), i.end()));
    #endif
    #ifdef BOOST_POSIX_API
        std::vector<std::string> sargs;
        for(auto i : args)
            ;// XXX sargs.push_back(toStdString(i));
    #endif

    try {
        auto e_ptr = std::make_shared<std::exception_ptr>();
        waitForExitThread = boost::thread([=](){
            try {
                auto child = process::execute(
                    #if defined BOOST_POSIX_API
                        set_args(sargs),
                        set_env(env),
                    #elif defined BOOST_WINDOWS_API
                        set_args(args),
                        set_env(wenv),
                    #endif
                    bind_stdout(*stdout_sink),
                    bind_stderr(*stderr_sink),
                    close_stdin(),
                    #ifdef BOOST_POSIX_API
                        notify_io_service(service),
                    #endif
                    hide_console(),
                    throw_on_error()
                );
                #if defined BOOST_POSIX_API
                    terminate_func = [child]() { boost::system::error_code ec; process::terminate(child, ec); };
                #elif defined BOOST_WINDOWS_API
                    HANDLE native_handle = child.process_handle();
                    terminate_func = [native_handle]() { TerminateProcess(native_handle, EXIT_FAILURE); };
                #endif

                // Now who would have guessed that calling WaitForSingleObject() on a process
                // in a thread other than the thread where you created the process doesn't work?
                // Certainly not MSDN docs, that's for sure. No mention of that there.
                // Man I hate WinAPI.
                boost::system::error_code ec;
                returnCode = process::wait_for_exit(child, ec);
                if(returnCode != 0)
                    logger.warning("Process ", cmd, " finished with error code ", returnCode);

                // Close the streams and let the io_service thread send a termination message
                // once it's done reading the data.
                #if defined BOOST_POSIX_API
                    stdout_sink->close();
                    stderr_sink->close();
                #elif defined BOOST_WINDOWS_API
                    for(auto i : { stdout_pipe_sink, stderr_pipe_sink })
                        CloseHandle(i);
                #endif

            } catch(...) {
                *e_ptr = std::current_exception();
            }
        });
        if(waitForExitThread.try_join_for(boost::chrono::milliseconds(100)) && *e_ptr)
            std::rethrow_exception(*e_ptr);
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
    *onRead = [=](const boost::system::error_code& ec, std::size_t /* bytes */){
        if(!ec) {
            std::istream is(stdoutBuf.get());
            std::string msg;
            std::getline(is, msg);
            // XXX QCoreApplication::postEvent(eventReceiver, new ReadEvent(cmd, msg));
            asio::async_read_until(*stdout_pend, *stdoutBuf, &matchNewline, *onRead);
        }
    };
    *onErrRead = [=](const boost::system::error_code& ec, std::size_t /* bytes */){
        if(!ec) {
            std::istream is(stderrBuf.get());
            std::string msg;
            std::getline(is, msg);
            // XXX QCoreApplication::postEvent(eventReceiver, new ReadEvent(cmd, msg));
            asio::async_read_until(*stderr_pend, *stderrBuf, &matchNewline, *onErrRead);
        }
    };
    asio::async_read_until(*stdout_pend, *stdoutBuf, &matchNewline, *onRead);
    asio::async_read_until(*stderr_pend, *stderrBuf, &matchNewline, *onErrRead);

    runServiceThread = boost::thread(boost::bind(&ProcessRunner::runService, this));
}

void ProcessRunner::terminate() {
    try {
        terminate_func();
    } catch(boost::system::system_error e) {
        logger.warning("Error terminating process ", cmd, ": ", e.what());
    }
}

ProcessRunner::~ProcessRunner() {
    service.stop();
    if(runServiceThread.joinable())
        runServiceThread.join();
    if(waitForExitThread.joinable())
        waitForExitThread.join();
}

ProcessRunner::ProcessRunner(QObject* eventReceiver, Logger& logger, const std::string& cmd,
        const std::vector<std::wstring>& args) : eventReceiver(eventReceiver), logger(logger), cmd(cmd), args(args) {
}

ProcessRunner::ProcessRunner(ProcessRunner&& p) : eventReceiver(p.eventReceiver), logger(p.logger), cmd(p.cmd), args(p.args) {}

void ProcessRunner::runService() {
    service.run();
    if(waitForExitThread.joinable())
        waitForExitThread.join();
    // XXX QCoreApplication::postEvent(eventReceiver, new TerminateEvent(cmd, returnCode));
}
