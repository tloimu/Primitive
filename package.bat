call build-vars.bat

set EXE_DIR=E:/Unreal Projects/%PROJECT_NAME%Exe
set UPROJECT=%PROJECT_DIR%\%PROJECT_NAME%.uproject
set UCONFIG=Development
set UMAPS=VoxelMap
"%UE_UAT%" -ScriptsForProject="%UPROJECT%" Turnkey -command=VerifySdk -platform=Win64 -UpdateIfNeeded -EditorIO -EditorIOPort=56443 -project="%UPROJECT%" BuildCookRun -nop4 -utf8output -nocompileeditor -skipbuildeditor -cook -project="%UPROJECT%" -target=%PROJECT_NAME% -unrealexe="%UE_EDITOR_CMD%" -platform=Win64 -installed -stage -archive -package -build -pak -iostore -compressed -prereqs -archivedirectory="%EXE_DIR%" -clientconfig=%UCONFIG% -nocompile -nocompileuat -maps=%UMAPS%
