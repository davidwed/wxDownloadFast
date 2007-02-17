; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=wxDownload Fast
AppVerName=wxDownload Fast 0.5.5
AppPublisher=Max Velasques
AppPublisherURL=http://dfast.sourceforge.net
AppSupportURL=http://dfast.sourceforge.net
AppUpdatesURL=http://dfast.sourceforge.net
DefaultDirName={pf}\wxDownload Fast
DefaultGroupName=wxDownload Fast
AllowNoIcons=yes
InfoAfterFile=..\README
LicenseFile=..\COPYING
Compression=lzma
SolidCompression=yes

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
; Name: "pt"; MessagesFile: "compiler:Languages\Portuguese-BR.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\src\mingwm10.dll"; DestDir: "{app}";
Source: "..\src\wxDFast.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\README"; DestDir: "{app}"; Flags: isreadme
Source: "..\README.br"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\ChangeLog"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\ChangeLog.br"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\TODO"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\COPYING"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\languages\cs\wxDFast.mo"; DestDir: "{app}\languages\cs"; Flags: ignoreversion
Source: "..\languages\cs\wxDFast.po"; DestDir: "{app}\languages\cs"; Flags: ignoreversion
Source: "..\languages\de\wxDFast.mo"; DestDir: "{app}\languages\de"; Flags: ignoreversion
Source: "..\languages\de\wxDFast.po"; DestDir: "{app}\languages\de"; Flags: ignoreversion
Source: "..\languages\es\wxDFast.mo"; DestDir: "{app}\languages\es"; Flags: ignoreversion
Source: "..\languages\es\wxDFast.po"; DestDir: "{app}\languages\es"; Flags: ignoreversion
Source: "..\languages\fr\wxDFast.mo"; DestDir: "{app}\languages\fr"; Flags: ignoreversion
Source: "..\languages\fr\wxDFast.po"; DestDir: "{app}\languages\fr"; Flags: ignoreversion
Source: "..\languages\hu\wxDFast.mo"; DestDir: "{app}\languages\hu"; Flags: ignoreversion
Source: "..\languages\hu\wxDFast.po"; DestDir: "{app}\languages\hu"; Flags: ignoreversion
Source: "..\languages\id\wxDFast.mo"; DestDir: "{app}\languages\id"; Flags: ignoreversion
Source: "..\languages\id\wxDFast.po"; DestDir: "{app}\languages\id"; Flags: ignoreversion
Source: "..\languages\pl\wxDFast.mo"; DestDir: "{app}\languages\pl"; Flags: ignoreversion
Source: "..\languages\pl\wxDFast.po"; DestDir: "{app}\languages\pl"; Flags: ignoreversion
Source: "..\languages\pt_BR\wxDFast.mo"; DestDir: "{app}\languages\pt_BR"; Flags: ignoreversion
Source: "..\languages\pt_BR\wxDFast.po"; DestDir: "{app}\languages\pt_BR"; Flags: ignoreversion
Source: "..\languages\ru\wxDFast.mo"; DestDir: "{app}\languages\ru"; Flags: ignoreversion
Source: "..\languages\ru\wxDFast.po"; DestDir: "{app}\languages\ru"; Flags: ignoreversion
Source: "..\languages\tr\wxDFast.mo"; DestDir: "{app}\languages\tr"; Flags: ignoreversion
Source: "..\languages\tr\wxDFast.po"; DestDir: "{app}\languages\tr"; Flags: ignoreversion
Source: "..\resources\RipStop\icon\wxdfast.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\resources\RipStop\logo\about.png"; DestDir: "{app}\resources\RipStop\logo"; Flags: ignoreversion
Source: "..\resources\RipStop\icon\wxdfast.png"; DestDir: "{app}\resources\RipStop\icon"; Flags: ignoreversion
Source: "..\resources\RipStop\menubar\*.png"; DestDir: "{app}\resources\RipStop\menubar"; Flags: ignoreversion
Source: "..\resources\RipStop\toolbar\*.png"; DestDir: "{app}\resources\RipStop\toolbar"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[REGISTRY]
Root: HKCU; Subkey: "Software\wxWidgets Program"; Flags: uninsdeletekeyifempty
Root: HKCU; Subkey: "Software\wxWidgets Program\wxDownload Fast"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\wxWidgets Program"; Flags: uninsdeletekeyifempty
Root: HKLM; Subkey: "Software\wxWidgets Program\wxDownload Fast"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\wxWidgets Program\wxDownload Fast"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"
Root: HKLM; Subkey: "Software\wxWidgets Program\wxDownload Fast"; ValueType: string; ValueName: "ExePath"; ValueData: "{app}\wxDFast.exe"

[INI]
Filename: "{app}\wxDFast.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://dfast.sourceforge.net"

[Icons]
Name: "{group}\wxDownload Fast"; Filename: "{app}\wxDFast.exe"; WorkingDir:"{app}"
Name: "{group}\{cm:ProgramOnTheWeb,wxDownload Fast}"; Filename: "{app}\wxDFast.url"; WorkingDir:"{app}"
Name: "{group}\{cm:UninstallProgram,wxDownload Fast}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\wxDownload Fast"; Filename: "{app}\wxDFast.exe"; WorkingDir:"{app}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\wxDownload Fast"; Filename: "{app}\wxDFast.exe"; WorkingDir:"{app}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\wxDFast.exe"; Description: "{cm:LaunchProgram,wxDownload Fast}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\wxDFast.url"

