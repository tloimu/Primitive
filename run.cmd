call build-vars.bat

"%UE_EDITOR%" "%PROJECT_DIR%\%PROJECT_NAME%.uproject" -game -log -WINDOWED -ResX=640 -ResY=480 -nosteam
