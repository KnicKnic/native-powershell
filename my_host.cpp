//https://docs.microsoft.com/en-us/powershell/developer/hosting/host02-sample 

  using namespace System;
  using namespace System::Collections::Generic;
  using namespace System::Globalization;
  using namespace System::Management::Automation;
  using namespace System::Management::Automation::Host;

  ref class MyRawUserInterface : PSHostRawUserInterface {
      // Inherited via PSHostRawUserInterface
  public:
      virtual property System::ConsoleColor BackgroundColor {
          System::ConsoleColor get() override { throw gcnew System::NotImplementedException(); };
          void set(System::ConsoleColor) override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::Management::Automation::Host::Size BufferSize {
          System::Management::Automation::Host::Size get() override { throw gcnew System::NotImplementedException(); };
          void set(System::Management::Automation::Host::Size) override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::Management::Automation::Host::Coordinates CursorPosition {
          System::Management::Automation::Host::Coordinates get() override { throw gcnew System::NotImplementedException(); };
          void set(System::Management::Automation::Host::Coordinates) override { throw gcnew System::NotImplementedException(); };
      }
      virtual property int CursorSize {
          int get() override { throw gcnew System::NotImplementedException(); };
          void set(int) override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::ConsoleColor ForegroundColor {
          System::ConsoleColor get() override { throw gcnew System::NotImplementedException(); };
          void set(System::ConsoleColor) override { throw gcnew System::NotImplementedException(); };
      }
      virtual property bool KeyAvailable {
          bool get() override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::Management::Automation::Host::Size MaxPhysicalWindowSize {
          System::Management::Automation::Host::Size  get() override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::Management::Automation::Host::Size MaxWindowSize {
          System::Management::Automation::Host::Size get() override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::Management::Automation::Host::Coordinates WindowPosition {
          System::Management::Automation::Host::Coordinates get() override { throw gcnew System::NotImplementedException(); };
          void set(System::Management::Automation::Host::Coordinates) override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::Management::Automation::Host::Size WindowSize {
          System::Management::Automation::Host::Size get() override { throw gcnew System::NotImplementedException(); };
          void set(System::Management::Automation::Host::Size) override { throw gcnew System::NotImplementedException(); };
      }
      virtual property System::String^ WindowTitle {
          System::String^ get() override { return ""; };
          void set(System::String^) override { };
      }
      virtual System::Management::Automation::Host::KeyInfo ReadKey(System::Management::Automation::Host::ReadKeyOptions options) override
      {
          return System::Management::Automation::Host::KeyInfo();
      }
      virtual void FlushInputBuffer() override
      {
          throw gcnew System::NotImplementedException();
      }
      virtual void SetBufferContents(System::Management::Automation::Host::Coordinates origin, array<System::Management::Automation::Host::BufferCell, 2>^ contents) override
      {
          throw gcnew System::NotImplementedException();
      }
      virtual void SetBufferContents(System::Management::Automation::Host::Rectangle rectangle, System::Management::Automation::Host::BufferCell fill) override
      {
          throw gcnew System::NotImplementedException();
      }
      virtual array<System::Management::Automation::Host::BufferCell, 2>^ GetBufferContents(System::Management::Automation::Host::Rectangle rectangle) override
      {
          throw gcnew System::NotImplementedException();
          // TODO: insert return statement here
      }
      virtual void ScrollBufferContents(System::Management::Automation::Host::Rectangle source, System::Management::Automation::Host::Coordinates destination, System::Management::Automation::Host::Rectangle clip, System::Management::Automation::Host::BufferCell fill) override
      {
          throw gcnew System::NotImplementedException();
      }
  };


  /// <summary>
  /// A sample implementation of the PSHostUserInterface abstract class for
  /// console applications. Not all members are implemented. Those that are
  /// not implemented throw a NotImplementedException exception. Members that
  /// are implemented include those that map easily to Console APIs.
  /// </summary>
  ref class MyHostUserInterface : PSHostUserInterface
  {
      /// <summary>
      /// Writes characters to the output display of the host.
      /// </summary>
      /// <param name="value">The characters to be written.</param>
  public:
      virtual void Write(System::String^ value)override
      {
          System::Console::Write(value);
      }

      /// <summary>
      /// Writes characters to the output display of the host and specifies the
      /// foreground and background colors of the characters. This implementation
      /// ignores the colors.
      /// </summary>
      /// <param name="foregroundColor">The color of the characters.</param>
      /// <param name="backgroundColor">The background color to use.</param>
      /// <param name="value">The characters to be written.</param>
      virtual void Write(
          ConsoleColor foregroundColor,
          ConsoleColor backgroundColor,
          System::String^ value)override
      {
          // Colors are ignored.
          System::Console::Write(value);
      }

      /// <summary>
      /// Writes a debug message to the output display of the host.
      /// </summary>
      /// <param name="message">The debug message that is displayed.</param>
      virtual void WriteDebugLine(System::String^ message)override
      {
          System::Console::WriteLine(System::String::Format(
              CultureInfo::CurrentCulture,
              "DEBUG: {0}",
              message));
      }

      /// <summary>
      /// Writes an error message to the output display of the host.
      /// </summary>
      /// <param name="value">The error message that is displayed.</param>
      virtual void WriteErrorLine(System::String^ value)override
      {
          System::Console::WriteLine(System::String::Format(
              CultureInfo::CurrentCulture,
              "ERROR: {0}",
              value));
      }

      /// <summary>
      /// Writes a newline character (carriage return)
      /// to the output display of the host.
      /// </summary>
      virtual void WriteLine()override
      {
          System::Console::WriteLine();
      }

      /// <summary>
      /// Writes a line of characters to the output display of the host
      /// and appends a newline character(carriage return).
      /// </summary>
      /// <param name="value">The line to be written.</param>
      virtual void WriteLine(System::String^ value)override
      {
          System::Console::WriteLine(value);
      }

      /// <summary>
      /// Writes a line of characters to the output display of the host
      /// with foreground and background colors and appends a newline (carriage return).
      /// </summary>
      /// <param name="foregroundColor">The foreground color of the display. </param>
      /// <param name="backgroundColor">The background color of the display. </param>
      /// <param name="value">The line to be written.</param>
      virtual void WriteLine(ConsoleColor foregroundColor, ConsoleColor backgroundColor, System::String^ value)override
      {
          // Write to the output stream, ignore the colors
          System::Console::WriteLine(value);
      }

      /// <summary>
      /// Writes a progress report to the output display of the host.
      /// </summary>
      /// <param name="sourceId">Unique identifier of the source of the record. </param>
      /// <param name="record">A ProgressReport object.</param>
      virtual void WriteProgress(Int64 , ProgressRecord^ )override
      {
      }

      /// <summary>
      /// Writes a verbose message to the output display of the host.
      /// </summary>
      /// <param name="message">The verbose message that is displayed.</param>
      virtual void WriteVerboseLine(System::String^ message)override
      {
          System::Console::WriteLine(System::String::Format(CultureInfo::CurrentCulture, "VERBOSE: {0}", message));
      }

      /// <summary>
      /// Writes a warning message to the output display of the host.
      /// </summary>
      /// <param name="message">The warning message that is displayed.</param>
      virtual void WriteWarningLine(System::String^ message)override
      {
          System::Console::WriteLine(System::String::Format(CultureInfo::CurrentCulture, "WARNING: {0}", message));
      }

      private:
          MyRawUserInterface^ myRawUserInterface = gcnew MyRawUserInterface();
    public:
      // Inherited via PSHostUserInterface
        virtual property System::Management::Automation::Host::PSHostRawUserInterface^ RawUI {
            System::Management::Automation::Host::PSHostRawUserInterface^ get() override { return this->myRawUserInterface; }
      }
      virtual System::String^ ReadLine() override
      {
          throw gcnew System::NotImplementedException();
          // TODO: insert return statement here
      }
      virtual System::Security::SecureString^ ReadLineAsSecureString() override
      {
          throw gcnew System::NotImplementedException();
          // TODO: insert return statement here
      }
      virtual System::Collections::Generic::Dictionary<System::String^, System::Management::Automation::PSObject^>^ Prompt(System::String^ caption, System::String^ message, System::Collections::ObjectModel::Collection<System::Management::Automation::Host::FieldDescription^>^ descriptions) override
      {
          throw gcnew System::NotImplementedException();
          // TODO: insert return statement here
      }
      virtual System::Management::Automation::PSCredential^ PromptForCredential(System::String^ caption, System::String^ message, System::String^ userName, System::String^ targetName) override
      {
          throw gcnew System::NotImplementedException();
          // TODO: insert return statement here
      }
      virtual System::Management::Automation::PSCredential^ PromptForCredential(System::String^ caption, System::String^ message, System::String^ userName, System::String^ targetName, System::Management::Automation::PSCredentialTypes allowedCredentialTypes, System::Management::Automation::PSCredentialUIOptions options) override
      {
          throw gcnew System::NotImplementedException();
          // TODO: insert return statement here
      }
      virtual int PromptForChoice(System::String^ caption, System::String^ message, System::Collections::ObjectModel::Collection<System::Management::Automation::Host::ChoiceDescription^>^ choices, int defaultChoice) override
      {
          throw gcnew System::NotImplementedException();
      }
  };


  ref class MyHost : PSHost
  {

      /// <summary>
      /// A reference to the implementation of the PSHostUserInterface
      /// class for this application.
      /// </summary>
  private: 

       CultureInfo^ originalCultureInfo =
          System::Threading::Thread::CurrentThread->CurrentCulture;

       MyHostUserInterface^ myHostUserInterface = gcnew MyHostUserInterface();
      /// <summary>
      /// The UI culture information of the thread that created
      /// this object.
      /// </summary>
       CultureInfo^ originalUICultureInfo =
          System::Threading::Thread::CurrentThread->CurrentUICulture;

      /// <summary>
      /// The identifier of this PSHost implementation.
      /// </summary>
       Guid myId = Guid::NewGuid();

  public:

      /// <summary>
      /// Initializes a new instance of the MyHost class. Keep
      /// a reference to the host application object so that it
      /// can be informed of when to exit.
      /// </summary>
      /// <param name="program">
      /// A reference to the host application object.
      /// </param>
      MyHost()
      {
      }

      /// <summary>
      /// Gets a string that contains the name of this host implementation.
      /// Keep in mind that this string may be used by script writers to
      /// identify when your host is being used.
      /// </summary>
      virtual property System::String^ Name
      {
          System::String^ get() override { return "MySampleConsoleHostImplementation"; }
      }

      /// <summary>
      /// Gets an instance of the implementation of the PSHostUserInterface
      /// class for this application. This instance is allocated once at startup time
      /// and returned every time thereafter.
      /// </summary>
      virtual property PSHostUserInterface^ UI
      {
          PSHostUserInterface^ get() override { return this->myHostUserInterface; }
      }


      /// <summary>
      /// Gets the culture information to use. This implementation
      /// returns a snapshot of the culture information of the thread
      /// that created this object.
      /// </summary>
      virtual  property System::Globalization::CultureInfo^ CurrentCulture
      {
        System::Globalization::CultureInfo^  get() override { return this->originalCultureInfo; }
      }

          /// <summary>
          /// Gets the UI culture information to use. This implementation
          /// returns a snapshot of the UI culture information of the thread
          /// that created this object.
          /// </summary>
      virtual property System::Globalization::CultureInfo^ CurrentUICulture
      {
          System::Globalization::CultureInfo^  get() override { return this->originalUICultureInfo; }
      }

          //System::Management::Automation::Host::PSHost::InstanceId 
          /// <summary>
          /// Gets an identifier for this host. This implementation always
          /// returns the GUID allocated at instantiation time.
          /// </summary>
      virtual property Guid InstanceId
      {
              Guid get() override { return this->myId; }
      }

          /// <summary>
          /// Gets the version object for this application. Typically this
          /// should match the version resource in the application.
          /// </summary>
      virtual property System::Version^ Version
      {
              System::Version^ get() override { return gcnew System::Version(1, 0, 0, 0); }
      }


      virtual void SetShouldExit(int exitCode) override
      {
          System::Console::WriteLine(System::String::Format("ERROR app tried to exit with exit code {0}", exitCode));
          throw gcnew System::NotImplementedException();
      }

      virtual void EnterNestedPrompt() override
      {
          throw gcnew System::NotImplementedException();
      }

      virtual void ExitNestedPrompt() override
      {
          throw gcnew System::NotImplementedException();
      }

      virtual void NotifyBeginApplication() override
      {
          throw gcnew System::NotImplementedException();
      }

      virtual void NotifyEndApplication() override
      {
          throw gcnew System::NotImplementedException();
      }

  };

 
