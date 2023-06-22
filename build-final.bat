call build-vars.bat

REM "%UE_BUILD%" "%PROJECT_NAME%" Win64 Development "%PROJECT_DIR%\%PROJECT_NAME%.uproject" -waitmutex -NoHotReload

"%UE_EDITOR%" "%PROJECT_DIR%\%PROJECT_NAME%.uproject" -run=cook -targetPlatform=WindowsNoEditor


