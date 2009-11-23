@echo off
rem called with c:\projects\gitproxy.cmd github.com 9418
rem plink -load gitproxy git@%1:%2
rem plink -load github %1 %2 %3 %4 %5 %6 %7 %8 %9

C:\Projects\git\connect\connect -S localhost:4001 %1 %2