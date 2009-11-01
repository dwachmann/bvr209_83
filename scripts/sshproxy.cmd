@echo off
rem echo call parameter:%0 %1 %2 %3 %4 %5 %6 %7 %8 %9 >> C:\Projects\git\sshproxy.out
"c:\program files\putty\plink" -v -load github %1 %2 %3 %4 %5 %6 %7 %8 %9
