@echo off
echo call parameter:%0 %1 %2 %3 %4 %5 %6 %7 %8 %9 >> C:\Projects\git\sshproxy.out
"C:\Program Files\putty\plink.exe" -load gitproxy -ssh -v "%1" %2 %3 %4 %5 %6 %7 %8 %9
