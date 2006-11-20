rem -------------------------------------------------------------------------
rem create-windows.sh
rem
rem Create a Windows directory for the installer to use.
rem
rem Copyright 2006 Michael W Thelen <mthelen@gmail.com>.
rem
rem This file is part of Zyzzyva.
rem
rem Zyzzyva is free software; you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation; either version 2 of the License, or
rem (at your option) any later version.
rem
rem Zyzzyva is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with this program; if not, write to the Free Software
rem Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
rem -------------------------------------------------------------------------

setlocal

set QTVER=4.1.5
set QTDIR=C:\Qt\%QTVER%
set OUTDIR=installer\windows

qmake
make

mkdir %OUTDIR%

rem Copy Qt libraries
copy %QTDIR%\bin\assistant.exe %OUTDIR%
copy %QTDIR%\bin\QtCore4.dll %OUTDIR%
copy %QTDIR%\bin\QtGui4.dll %OUTDIR%
copy %QTDIR%\bin\QtNetwork4.dll %OUTDIR%
copy %QTDIR%\bin\QtSql4.dll %OUTDIR%
copy %QTDIR%\bin\QtXml4.dll %OUTDIR%
copy %QTDIR%\bin\mingwm10.dll %OUTDIR%

rem Copy Qt plugins
mkdir %OUTDIR%\sqldrivers
copy %QTDIR%\plugins\sqldrivers\qsqlite.dll %OUTDIR%\sqldrivers

rem Copy Zyzzyva stuff
copy bin\zyzzyva.exe %OUTDIR%
copy bin\zyzzyva1.dll %OUTDIR%
copy zyzzyva.top %OUTDIR%
copy AUTHORS %OUTDIR%\AUTHORS.txt
copy CHANGES %OUTDIR%\CHANGES.txt
copy COPYING %OUTDIR%\COPYING.txt
copy LICENSE %OUTDIR%\LICENSE.txt
copy README %OUTDIR%\README.txt

rem Copy Zyzzyva data directory
xcopy /I /E /H data %OUTDIR%\data
rmdir /s /q "%OUTDIR%\data\.svn"
rmdir /s /q "%OUTDIR%\data\help\.svn"
rmdir /s /q "%OUTDIR%\data\judge\.svn"
rmdir /s /q "%OUTDIR%\data\judge\saved\.svn"
rmdir /s /q "%OUTDIR%\data\quiz\.svn"
rmdir /s /q "%OUTDIR%\data\quiz\predefined\.svn"
rmdir /s /q "%OUTDIR%\data\quiz\saved\.svn"
rmdir /s /q "%OUTDIR%\data\search\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\Hooks\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\JQXZ\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\Prefixes\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\Suffixes\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\Top 7s By Stem\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\Top 8s By Stem\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\Vowel Dumps\.svn"
rmdir /s /q "%OUTDIR%\data\search\predefined\Word Length\.svn"
rmdir /s /q "%OUTDIR%\data\search\saved\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\black-pro\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\blue\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\blue-with-border\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\brown\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\gray\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\green\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\purple\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\tan\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\tan-with-border\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\white-pro\.svn"
rmdir /s /q "%OUTDIR%\data\tiles\white-pro-border\.svn"
rmdir /s /q "%OUTDIR%\data\words\.svn"
rmdir /s /q "%OUTDIR%\data\words\british\.svn"
rmdir /s /q "%OUTDIR%\data\words\french\.svn"
rmdir /s /q "%OUTDIR%\data\words\north-american\.svn"
rmdir /s /q "%OUTDIR%\data\words\saved\.svn"

endlocal
