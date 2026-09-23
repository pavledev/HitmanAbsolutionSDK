#include "Logging.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/msvc_sink.h"

#include "ModSDK.h"
#include "UI/Console.h"

static std::vector<spdlog::logger*>* g_Loggers;

template<class Mutex> class ConsoleSink : public spdlog::sinks::base_sink<Mutex>
{
  public:
    ConsoleSink() {}

    void sink_it_(const spdlog::details::log_msg& p_Message) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(p_Message, formatted);

        ModSDK::GetInstance().GetUIConsole()->AddLogLine(p_Message.level, std::string(formatted.data(), formatted.size()));
    }

    void flush_() override {}
};

using ConsoleSinkST = ConsoleSink<spdlog::details::null_mutex>;
using ConsoleSinkMT = ConsoleSink<std::mutex>;

void DispatchLog(spdlog::level::level_enum p_Level, std::string_view p_Msg)
{
    if (g_Loggers == nullptr)
    {
        return;
    }

    for (auto* logger : *g_Loggers)
    {
        logger->log(p_Level, fmt::string_view(p_Msg.data(), p_Msg.size()));
    }
}

void ClearLoggers()
{
    if (g_Loggers == nullptr)
    {
        return;
    }

    for (auto& logger : *g_Loggers)
    {
        delete logger;
    }

    g_Loggers->clear();

    delete g_Loggers;
    g_Loggers = nullptr;
}

void SetupLogging(spdlog::level::level_enum p_LogLevel)
{
    ClearLoggers();

    if (g_Loggers == nullptr)
    {
        g_Loggers = new std::vector<spdlog::logger*>();
    }

    auto consoleDistSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
    auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto uiConsoleSink = std::make_shared<ConsoleSinkMT>();

#if _DEBUG
    auto debugSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    consoleDistSink->add_sink(debugSink);
#endif

    consoleDistSink->add_sink(stdoutSink);
    consoleDistSink->add_sink(uiConsoleSink);

    auto consoleLogger = new spdlog::logger("con", consoleDistSink);

    consoleLogger->set_level(p_LogLevel);
    consoleLogger->set_pattern("%v");

    g_Loggers->push_back(consoleLogger);

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("HitmanAbsolutionSDK.log", true);

    auto fileLogger = new spdlog::logger("file", fileSink);

    fileLogger->set_level(p_LogLevel);
    fileLogger->set_pattern("%v");
    fileLogger->flush_on(spdlog::level::trace);

    g_Loggers->push_back(fileLogger);
}

void FlushLoggers()
{
    if (g_Loggers == nullptr)
    {
        return;
    }

    for (auto* logger : *g_Loggers)
    {
        logger->flush();
    }
}
