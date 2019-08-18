#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include "host.h"
#include <string>
ref class Logger {
public:
    Logger(void * contextParam, NativePowerShell_LogString WriteFunc) : BaseLogString(WriteFunc), context(contextParam) {
        LogWarningDelegate = gcnew WriteLoggerDelegate(this, &Logger::LogWarning);
        LogInformationDelegate = gcnew WriteLoggerDelegate(this, &Logger::LogInformation);
        LogVerboseDelegate = gcnew WriteLoggerDelegate(this, &Logger::LogVerbose);
        LogDebugDelegate = gcnew WriteLoggerDelegate(this, &Logger::LogDebug);
        LogErrorDelegate = gcnew WriteLoggerDelegate(this, &Logger::LogError);
        LogDelegate = gcnew WriteLoggerDelegate(this, &Logger::Log);
    };
    NativePowerShell_LogString BaseLogString = nullptr;
    NativePowerShell_LogString LogWarningPtr = nullptr;
    NativePowerShell_LogString LogInformationPtr = nullptr;
    NativePowerShell_LogString LogVerbosePtr = nullptr;
    NativePowerShell_LogString LogDebugPtr = nullptr;
    NativePowerShell_LogString LogErrorPtr = nullptr;

    NativePowerShell_LogString BaseLogLinePtr = nullptr;
    NativePowerShell_LogString LogWarningLinePtr = nullptr;
    NativePowerShell_LogString LogInformationLinePtr = nullptr;
    NativePowerShell_LogString LogVerboseLinePtr = nullptr;
    NativePowerShell_LogString LogDebugLinePtr = nullptr;
    NativePowerShell_LogString LogErrorLinePtr = nullptr;

    void LogWarning(const std::wstring& log);
    void LogInformation(const std::wstring& log);
    void LogVerbose(const std::wstring& log);
    void LogDebug(const std::wstring& log);
    void LogError(const std::wstring& log);
    void Log(const std::wstring& log);

    void LogLineWarning(const std::wstring& log);
    void LogLineInformation(const std::wstring& log);
    void LogLineVerbose(const std::wstring& log);
    void LogLineDebug(const std::wstring& log);
    void LogLineError(const std::wstring& log);
    void LogLine(const std::wstring& log);


    void LogWarning(System::String^ log);
    void LogInformation(System::String^ log);
    void LogVerbose(System::String^ log);
    void LogDebug(System::String^ log);
    void LogError(System::String^ log);
    void Log(System::String^ log);

    void LogLineWarning(System::String^ log);
    void LogLineInformation(System::String^ log);
    void LogLineVerbose(System::String^ log);
    void LogLineDebug(System::String^ log);
    void LogLineError(System::String^ log);
    void LogLine(System::String^ log);
private:
    void* context;
    delegate void WriteLoggerDelegate(const std::wstring&);

    WriteLoggerDelegate^ LogWarningDelegate;
    WriteLoggerDelegate^ LogInformationDelegate;
    WriteLoggerDelegate^ LogVerboseDelegate;
    WriteLoggerDelegate^ LogDebugDelegate;
    WriteLoggerDelegate^ LogErrorDelegate;
    WriteLoggerDelegate^ LogDelegate;

    void LogWrapperAddNewLine(NativePowerShell_LogString writeLine, WriteLoggerDelegate^ write, const std::wstring& log);
    //__inline void LogWrapperAddNewLine(NativePowerShell_LogString writeLine, void (Logger::* write)(const std::wstring&), const std::wstring& log) {
    //    if (writeLine == nullptr) {
    //        std::wstring message = log + L'\n';
    //        (this->*write)(message);
    //    }
    //    else {
    //        writeLine(log.c_str());
    //    }
    //}

    void LogWrapper(NativePowerShell_LogString func, LPCWSTR prepend, const std::wstring& log);
    /*__inline void LogWrapper(NativePowerShell_LogString func, LPCWSTR prepend, const std::wstring& log) {
        if (func == nullptr) {
            std::wstring message;
            if (prepend != nullptr)
                message = prepend;
            message = message + log;
            BaseLogString(message.c_str());
        }
        else {
            func(log.c_str());
        }
    }*/

};