#include <string>
#include "host_internal.h"
#include <msclr\marshal_cppstd.h>

void LogWrapper(LogString func, LPCWSTR prepend, const std::wstring& log) {
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
}

void LogWarning(const std::wstring& log) {
    LogWrapper(LogWarningPtr, L"Warning: ", log);
}
void LogInformation(const std::wstring& log) {
    LogWrapper(LogInformationPtr, L"Information: ", log);
}
void LogVerbose(const std::wstring& log) {
    LogWrapper(LogVerbosePtr, L"Verbose: ", log);
}
void LogDebug(const std::wstring& log) {
    LogWrapper(LogDebugPtr, L"Debug: ", log);
}
void LogError(const std::wstring& log) {
    LogWrapper(LogErrorPtr, L"Error: ", log);
}

void Log(const std::wstring& log) {
    BaseLogString(log.c_str());
}

void LogWrapperAddNewLine(LogString writeLine, void(*write)(const std::wstring&), const std::wstring& log) {
    if (writeLine == nullptr) {
        std::wstring message = log + L'\n';
        write(message);
    }
    else {
        writeLine(log.c_str());
    }
}


void LogLineWarning(const std::wstring& log) {
    LogWrapperAddNewLine(LogWarningPtr, LogWarning, log);
}
void LogLineInformation(const std::wstring& log) {
    LogWrapperAddNewLine(LogInformationPtr, LogInformation, log);
}
void LogLineVerbose(const std::wstring& log) {
    LogWrapperAddNewLine(LogVerbosePtr, LogVerbose, log);
}
void LogLineDebug(const std::wstring& log) {
    LogWrapperAddNewLine(LogDebugPtr, LogDebug, log);
}
void LogLineError(const std::wstring& log) {
    LogWrapperAddNewLine(LogErrorPtr, LogError, log);
}
void LogLine(const std::wstring& log) {
    LogWrapperAddNewLine(BaseLogString, Log, log);
}



void LogWarning(System::String^ log) { LogWarning(msclr::interop::marshal_as<std::wstring>(log)); }
void LogInformation(System::String^ log) { LogInformation(msclr::interop::marshal_as<std::wstring>(log)); }
void LogVerbose(System::String^ log) { LogVerbose(msclr::interop::marshal_as<std::wstring>(log)); }
void LogDebug(System::String^ log) { LogDebug(msclr::interop::marshal_as<std::wstring>(log)); }
void LogError(System::String^ log) { LogError(msclr::interop::marshal_as<std::wstring>(log)); }
void Log(System::String^ log) { Log(msclr::interop::marshal_as<std::wstring>(log)); }

void LogLineWarning(System::String^ log) { LogLineWarning(msclr::interop::marshal_as<std::wstring>(log)); }
void LogLineInformation(System::String^ log) { LogLineInformation(msclr::interop::marshal_as<std::wstring>(log)); }
void LogLineVerbose(System::String^ log) { LogLineVerbose(msclr::interop::marshal_as<std::wstring>(log)); }
void LogLineDebug(System::String^ log) { LogLineDebug(msclr::interop::marshal_as<std::wstring>(log)); }
void LogLineError(System::String^ log) { LogLineError(msclr::interop::marshal_as<std::wstring>(log)); }
void LogLine(System::String^ log) { LogLine(msclr::interop::marshal_as<std::wstring>(log)); }
