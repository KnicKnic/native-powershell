// https://docs.microsoft.com/en-us/powershell/developer/hosting/host02-sample 


  using namespace System;
  using namespace System::Collections::Generic;
  using namespace System::Globalization;
  using namespace System::Management::Automation;
  using namespace System::Management::Automation::Host;
#include "my_host.h"
#include "host_internal.h"


  inline void MyHostUserInterface::Write(System::String^ value)
  {
      GetLogger()->Log(value);
  }

  /// <summary>
  /// Writes characters to the output display of the host and specifies the
  /// foreground and background colors of the characters. This implementation
  /// ignores the colors.
  /// </summary>
  /// <param name="foregroundColor">The color of the characters.</param>
  /// <param name="backgroundColor">The background color to use.</param>
  /// <param name="value">The characters to be written.</param>

  inline void MyHostUserInterface::Write(ConsoleColor foregroundColor, ConsoleColor backgroundColor, System::String^ value)
  {
      // Colors are ignored.
      GetLogger()->Log(value);
  }

  /// <summary>
  /// Writes a debug message to the output display of the host.
  /// </summary>
  /// <param name="message">The debug message that is displayed.</param>

  inline void MyHostUserInterface::WriteDebugLine(System::String^ message)
  {
      GetLogger()->LogLineDebug(message);
  }

  /// <summary>
  /// Writes an error message to the output display of the host.
  /// </summary>
  /// <param name="value">The error message that is displayed.</param>

  inline void MyHostUserInterface::WriteErrorLine(System::String^ message)
  {
      GetLogger()->LogLineError(message);
  }

  /// <summary>
  /// Writes a newline character (carriage return)
  /// to the output display of the host.
  /// </summary>

  inline void MyHostUserInterface::WriteLine()
  {
      GetLogger()->LogLine(L"");
  }

  /// <summary>
  /// Writes a line of characters to the output display of the host
  /// and appends a newline character(carriage return).
  /// </summary>
  /// <param name="value">The line to be written.</param>

  inline void MyHostUserInterface::WriteLine(System::String^ message)
  {
      GetLogger()->LogLine(message);
  }

  /// <summary>
  /// Writes a line of characters to the output display of the host
  /// with foreground and background colors and appends a newline (carriage return).
  /// </summary>
  /// <param name="foregroundColor">The foreground color of the display. </param>
  /// <param name="backgroundColor">The background color of the display. </param>
  /// <param name="value">The line to be written.</param>

  inline void MyHostUserInterface::WriteLine(ConsoleColor foregroundColor, ConsoleColor backgroundColor, System::String^ message)
  {
      GetLogger()->LogLineDebug(message);
  }

  /// <summary>
  /// Writes a progress report to the output display of the host.
  /// </summary>
  /// <param name="sourceId">Unique identifier of the source of the record. </param>
  /// <param name="record">A ProgressReport object.</param>

  inline void MyHostUserInterface::WriteProgress(Int64, ProgressRecord^)
  {
  }

  /// <summary>
  /// Writes a verbose message to the output display of the host.
  /// </summary>
  /// <param name="message">The verbose message that is displayed.</param>

  inline void MyHostUserInterface::WriteVerboseLine(System::String^ message)
  {
      GetLogger()->LogLineVerbose(message);
  }

  /// <summary>
  /// Writes a warning message to the output display of the host.
  /// </summary>
  /// <param name="message">The warning message that is displayed.</param>

  inline void MyHostUserInterface::WriteWarningLine(System::String^ message)
  {
      GetLogger()->LogLineWarning(message);
  }

  inline System::String^ MyHostUserInterface::ReadLine()
  {
      throw gcnew System::NotImplementedException();
      // TODO: insert return statement here
  }

  inline System::Security::SecureString^ MyHostUserInterface::ReadLineAsSecureString()
  {
      throw gcnew System::NotImplementedException();
      // TODO: insert return statement here
  }

  inline System::Collections::Generic::Dictionary<System::String^, System::Management::Automation::PSObject^>^ MyHostUserInterface::Prompt(System::String^ caption, System::String^ message, System::Collections::ObjectModel::Collection<System::Management::Automation::Host::FieldDescription^>^ descriptions)
  {
      throw gcnew System::NotImplementedException();
      // TODO: insert return statement here
  }

  inline System::Management::Automation::PSCredential^ MyHostUserInterface::PromptForCredential(System::String^ caption, System::String^ message, System::String^ userName, System::String^ targetName)
  {
      throw gcnew System::NotImplementedException();
      // TODO: insert return statement here
  }

  inline System::Management::Automation::PSCredential^ MyHostUserInterface::PromptForCredential(System::String^ caption, System::String^ message, System::String^ userName, System::String^ targetName, System::Management::Automation::PSCredentialTypes allowedCredentialTypes, System::Management::Automation::PSCredentialUIOptions options)
  {
      throw gcnew System::NotImplementedException();
      // TODO: insert return statement here
  }

  inline int MyHostUserInterface::PromptForChoice(System::String^ caption, System::String^ message, System::Collections::ObjectModel::Collection<System::Management::Automation::Host::ChoiceDescription^>^ choices, int defaultChoice)
  {
      throw gcnew System::NotImplementedException();
  }


  inline void MyHost::SetShouldExit(int exitCode)
  {
      System::Console::WriteLine(System::String::Format("ERROR app tried to exit with exit code {0}", exitCode));
      throw gcnew System::NotImplementedException();
  }

  inline void MyHost::EnterNestedPrompt()
  {
      throw gcnew System::NotImplementedException();
  }

  inline void MyHost::ExitNestedPrompt()
  {
      throw gcnew System::NotImplementedException();
  }

  inline void MyHost::NotifyBeginApplication()
  {
      throw gcnew System::NotImplementedException();
  }

  inline void MyHost::NotifyEndApplication()
  {
      throw gcnew System::NotImplementedException();
  }

  inline System::Management::Automation::Host::KeyInfo MyRawUserInterface::ReadKey(System::Management::Automation::Host::ReadKeyOptions options)
  {
      return System::Management::Automation::Host::KeyInfo();
  }

  inline void MyRawUserInterface::FlushInputBuffer()
  {
      throw gcnew System::NotImplementedException();
  }

  inline void MyRawUserInterface::SetBufferContents(System::Management::Automation::Host::Coordinates origin, array<System::Management::Automation::Host::BufferCell, 2>^ contents)
  {
      throw gcnew System::NotImplementedException();
  }

  inline void MyRawUserInterface::SetBufferContents(System::Management::Automation::Host::Rectangle rectangle, System::Management::Automation::Host::BufferCell fill)
  {
      throw gcnew System::NotImplementedException();
  }

  inline array<System::Management::Automation::Host::BufferCell, 2>^ MyRawUserInterface::GetBufferContents(System::Management::Automation::Host::Rectangle rectangle)
  {
      throw gcnew System::NotImplementedException();
      // TODO: insert return statement here
  }

  inline void MyRawUserInterface::ScrollBufferContents(System::Management::Automation::Host::Rectangle source, System::Management::Automation::Host::Coordinates destination, System::Management::Automation::Host::Rectangle clip, System::Management::Automation::Host::BufferCell fill)
  {
      throw gcnew System::NotImplementedException();
  }

  inline Logger^ MyRawUserInterface::GetLogger() {
      return runspace->logger;
  }
  inline Logger^ MyHostUserInterface::GetLogger() {
      return runspace->logger;
  }

  inline Logger^ MyHost::GetLogger() {
      return runspace->logger;
  }
