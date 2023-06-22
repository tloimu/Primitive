set UE_DIR=D:\EpicGames\UE_5.2
set UE_EDITOR=%UE_DIR%\Engine\Binaries\Win64\UnrealEditor.exe
set UE_BUILD=%UE_DIR%\Engine\Build\BatchFiles\Build.bat

set PROJECT_DIR=%~dp0
set PROJECT_DIR=%PROJECT_DIR:~0,-1%

for %%I in (.) do set PROJECT_NAME=%%~nxI

set
