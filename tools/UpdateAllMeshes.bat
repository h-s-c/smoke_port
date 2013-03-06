if "%1"=="" goto PathNotGiven

for %%i in (*.mesh) do %1\tools\OgreMeshUpgrade.exe -b "%%i"
goto end


:PathNotGiven
for /f %%i in ("%0") do set toolpath=%%~dpi

for %%i in (*.mesh) do %toolpath:~0,-1%\OgreMeshUpgrade.exe -b "%%i"
goto end


:end