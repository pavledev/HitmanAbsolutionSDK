#include "Logging.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/msvc_sink.h"

#include "SDK.h"
#include "UI/Console.h"

template<class Mutex> class ConsoleSink : public spdlog::sinks::base_sink<Mutex>
{
  public:
    ConsoleSink() = default;

    void sink_it_(const spdlog::details::log_msg& p_Msg) override
    {
        spdlog::memory_buf_t formatted;
        this->formatter_->format(p_Msg, formatted);

        SDK::GetInstance().GetConsole()->AddLogLine(p_Msg.level, std::string(formatted.data(), formatted.size()));
    }

    void flush_() override {}
};

using ConsoleSink_mt = ConsoleSink<std::mutex>;

void SetupLogging(spdlog::level::level_enum p_LogLevel)
{
    spdlog::drop_all();

    auto distSink = std::make_shared<spdlog::sinks::dist_sink_mt>();

    auto stdoutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    distSink->add_sink(stdoutSink);

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("HitmanAbsolutionSDK.log", true);

    distSink->add_sink(fileSink);

    auto uiSink = std::make_shared<ConsoleSink_mt>();

    distSink->add_sink(uiSink);

#if _DEBUG
    auto debugSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();

    distSink->add_sink(debugSink);
#endif

    auto mainLogger = std::make_shared<spdlog::logger>("main", distSink);

    mainLogger->set_level(p_LogLevel);
    mainLogger->set_pattern("%v");

    spdlog::register_logger(mainLogger);
    spdlog::set_default_logger(mainLogger);
}

std::shared_ptr<spdlog::logger> GetMainLogger()
{
    return spdlog::get("main");
}
