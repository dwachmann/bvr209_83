@echo off

SET JAVA_HOME=c:\progra~1\java\jdk1.5.0_10
rem SET ANT_HOME=C:\Projects\3rdParty\apache-ant-1.6.1
SET ANT_HOME=C:\Projects\3rdParty\apache-ant-1.7.0
SET APPDIR=C:\Projects\smartcard\winapps

call "C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat"
call "C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\setEnv"

SET PATH=%PATH%;%JAVA_HOME%\bin;%ANT_HOME%\bin;c:\program files\ultraedit;c:\program files\git\bin

SET JC21_HOME=C:\Projects\smartcard\java_card_kit-2_1_2
SET JCOP_HOME=C:\Projects\smartcard\jcoptools
set OCF_INSTALLDIR=C:\Projects\smartcard\opencard\OCF1.2\
rem SET PATH=%PATH%;%JCOP_HOME%\bin;%JC21_HOME%\bin;c:\projects\3rdParty\cygwin\bin
SET PATH=%PATH%;%JCOP_HOME%\bin;%JC21_HOME%\bin
SET LIB=%LIB%;%OCF_INSTALLDIR%\lib

SET MSISCRIPTS="c:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Samples\SysMgmt\Msi\Scripts"
rem SET NODEBUG=1
SET GIT_SSH=c:\Projects\git\sshproxy.cmd
SET VISUAL=vim

cls
title Smartcard Environment
