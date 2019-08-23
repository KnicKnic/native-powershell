#include <string>
#include <msclr\marshal_cppstd.h>
#include "host.h"
#include "logger.hpp"


void Logger::LogWrapperAddNewLine(NativePowerShell_LogString writeLine, Logger::WriteLoggerDelegate^ write, const std::wstring& log) {
    if (writeLine == nullptr) {
        std::wstring message = log + L'\n';
        write(message);
    }
    else {
        writeLine(context, log.c_str());
    }
}

void Logger::LogWrapper(NativePowerShell_LogString func, LPCWSTR prepend, const std::wstring& log) {
    if (func == nullptr) {
        std::wstring message;
        if (prepend != nullptr)
            message = prepend;
        message = message + log;
        if (BaseLogString != nullptr) {
            BaseLogString(context, message.c_str());
        }
    }
    else {
        func(context, log.c_str());
    }
}

void Logger::LogWarning(const std::wstring & log) {
    LogWrapper(LogWarningPtr, L"Warning: ", log);
}
void Logger::LogInformation(const std::wstring & log) {
    LogWrapper(LogInformationPtr, L"Information: ", log);
}
void Logger::LogVerbose(const std::wstring & log) {
    LogWrapper(LogVerbosePtr, L"Verbose: ", log);
}
void Logger::LogDebug(const std::wstring & log) {
    LogWrapper(LogDebugPtr, L"Debug: ", log);
}
void Logger::LogError(const std::wstring & log) {
    LogWrapper(LogErrorPtr, L"Error: ", log);
}

void Logger::Log(const std::wstring & log) {
    if (BaseLogString != nullptr) {
        BaseLogString(context, log.c_str());
    }
}



void Logger::LogLineWarning(const std::wstring& log) {
    LogWrapperAddNewLine(LogWarningPtr, LogWarningDelegate, log);
}
void Logger::LogLineInformation(const std::wstring& log) {
    LogWrapperAddNewLine(LogInformationPtr, LogInformationDelegate, log);
}
void Logger::LogLineVerbose(const std::wstring& log) {
    LogWrapperAddNewLine(LogVerbosePtr, LogVerboseDelegate, log);
}
void Logger::LogLineDebug(const std::wstring& log) {
    LogWrapperAddNewLine(LogDebugPtr, LogDebugDelegate, log);
}
void Logger::LogLineError(const std::wstring& log) {
    LogWrapperAddNewLine(LogErrorPtr, LogErrorDelegate, log);
}
void Logger::LogLine(const std::wstring& log) {
    LogWrapperAddNewLine(BaseLogLinePtr, LogDelegate, log);
}




void Logger::LogWarning(System::String ^ log) { LogWarning(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogInformation(System::String ^ log) { LogInformation(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogVerbose(System::String ^ log) { LogVerbose(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogDebug(System::String ^ log) { LogDebug(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogError(System::String ^ log) { LogError(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::Log(System::String ^ log) { Log(msclr::interop::marshal_as<std::wstring>(log)); }

void Logger::LogLineWarning(System::String ^ log) { LogLineWarning(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogLineInformation(System::String ^ log) { LogLineInformation(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogLineVerbose(System::String ^ log) { LogLineVerbose(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogLineDebug(System::String ^ log) { LogLineDebug(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogLineError(System::String ^ log) { LogLineError(msclr::interop::marshal_as<std::wstring>(log)); }
void Logger::LogLine(System::String ^ log) { LogLine(msclr::interop::marshal_as<std::wstring>(log)); }