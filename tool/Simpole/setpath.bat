cd /d %~dp0
SET SF="HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders"
SET  curPath=%cd%


FOR /F "tokens=2,*" %%I IN ('REG QUERY %SF% /v Personal 2^>NUL^|FIND /I "Personal"') DO SET "myDoc=%%~J"
XCOPY startup.p  "%myDoc%\MATLAB\" /y


setx "SIMPOLE_HOME" %curPath%