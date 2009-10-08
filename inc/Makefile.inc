# $Id$
# 
# Copyright (C) 2008 Dorothea Wachmann
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.
#
# -------------------------------------------------------------------------
# This files should be included at the top of all MAKEFILEs as follows:
#  !include <Makefile.inc>
# -------------------------------------------------------------------------
#
# Define APPVER = [ 4.0 | 5.0 | 5.01 | 5.02 ] prior to including win32.mak to get
#  build time checking for version dependencies and to mark the executable
#  with version information.
#
# Define _WIN32_IE = [ 0x0300 | 0x0400 | 0x0500 | 0x0600] prior to including win32.mak to
#  get compile and link flags for building applications and components to
#  run on Internet Explorer. (This is an optional parameter.  IE 4.0 is
#  the default.)
#
# -------------------------------------------------------------------------
# NMAKE Options
#
# Use the table below to determine the additional options for NMAKE to
# generate various application debugging, profiling and performance tuning
# information.
#
# Application Information Type         Invoke NMAKE
# ----------------------------         ------------
# For No Debugging Info                nmake nodebug=1
# For Working Set Tuner Info           nmake tune=1
# For Call Attributed Profiling Info   nmake profile=1
#
# Note: The three options above are mutually exclusive (you may use only
#       one to compile/link the application).
#
# Note: creating the environment variables NODEBUG, TUNE, and PROFILE is an
#       alternate method to setting these options via the nmake command line.
#
# Note: TUNE and PROFILE do nothing for 64bit compilation
#
# Additional NMAKE Options             Invoke NMAKE
# ----------------------------         ------------
# For No ANSI NULL Compliance          nmake no_ansi=1
# (ANSI NULL is defined as PVOID 0)
#
# =========================================================================
# Build Rules Quick Start
#
# To build one of the following types of executables, use the specified
# compiler and linker command-line options.
#
#  ---------------------------------------------------------------------------
#   To build:         |  Compiler Options      | Linker options (pick one
#                     |                        | line. con = console,
#                     |                        | gui = GUI, ole = GUI OLE)
#  ---------------------------------------------------------------------------
#  Single threaded    | cdebug cflags cvars    | ldebug guilflags guilibs
#  app with static    |                        | ldebug conlflags conlibs
#  CRT                |                        | ldebug guilflags olelibs
#  ---------------------------------------------------------------------------
#  Multithreaded app  | cdebug cflags cvarsmt  | ldebug guilflags guilibsmt
#  with static CRT    |                        | ldebug conlflags conlibsmt
#                     |                        | ldebug guilflags olelibsmt
#  ---------------------------------------------------------------------------
#  Single or multi-   | cdebug cflags cvarsdll | ldebug guilflags guilibsdll
#  threaded app with  |                        | ldebug conlflags conlibsdll
#  DLL version of CRT |                        | ldebug guilflags olelibsdll
#  (MSVCRT.DLL)       |                        |
#  ---------------------------------------------------------------------------
#  DLL with static    | cdebug cflags cvarsmt  | ldebug dlllflags guilibsmt
#  CRT*               |                        | ldebug dlllflags conlibsmt
#                     |                        | ldebug dlllflags olelibsmt
#  ---------------------------------------------------------------------------
#  DLL with DLL       | cdebug cflags cvarsdll | ldebug dlllflags guilibsdll
#  version of CRT     |                        | ldebug dlllflags conlibsdll
#  (MSVCRT.DLL)       |                        | ldebug dlllflags olelibsdll
#  ---------------------------------------------------------------------------
#
#  * Always make DLLs multithreaded because a DLL has no way to know whether
#    the calling application has multiple threads, and has no way to prevent
#    multithreaded apps from loading it.
#
#  To specify an Intel x86 build that defaults to stdcall, add scall to the
#  list of compiler options.
#
# =========================================================================
!IFNDEF _MAKEFILE_INC_
_MAKEFILE_INC_ = 1

APPVER=5.01
_WIN32_IE=0x0600

SRCDIR=$(PROJECTDIR)\src
INCDIR=$(PROJECTDIR)\inc
RESDIR=$(PROJECTDIR)\res

# -------------------------------------------------------------------------
# Get CPU Type - exit if CPU environment variable is not defined
# -------------------------------------------------------------------------

# Win95 does not define PROCESSOR_ARCHITECTURE - default to i386

!IF "$(PROCESSOR_ARCHITECTURE)" == ""
CPU=i386
PROCESSOR_ARCHITECTURE=x86
!endif

!IF !DEFINED(CPU) || "$(CPU)" == ""
CPU = $(PROCESSOR_ARCHITECTURE)
!ENDIF # CPU

# if PROCESSOR_ARCHITECTURE was x86 or X86 change CPU to i386

!IF ( "$(CPU)" == "X86" ) || ( "$(CPU)" == "x86" )
CPU = i386
!ENDIF # CPU == X86

!IF "$(CPU)" != "i386"
!IF "$(CPU)" != "ALPHA"
!IF "$(CPU)" != "ALPHA64"
!IF "$(CPU)" != "IA64"
!IF "$(CPU)" != "AMD64"
!ERROR  Must specify CPU environment variable ( CPU=i386, CPU=ALPHA, CPU=IA64, CPU=ALPHA64, CPU=AMD64)
!ENDIF
!ENDIF
!ENDIF
!ENDIF
!ENDIF


# -------------------------------------------------------------------------
# Get Target Operating System - Default to WINNT
# -------------------------------------------------------------------------
# default to APPVER of 5.0

!IFNDEF APPVER
APPVER = 5.0
!ENDIF

!IF "$(APPVER)" != "5.02"
!IF "$(APPVER)" != "5.01"
!IF "$(APPVER)" != "5.0"
!IF "$(APPVER)" != "4.0"
!ERROR Must specify APPVER environment variable (4.0, 5.0, 5.01, 5.02)
!ENDIF
!ENDIF
!ENDIF
!ENDIF

!IF "$(APPVER)" =="5.0"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0500
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 5.0

!IF "$(APPVER)" =="5.01"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0600
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 5.01

!IF "$(APPVER)" =="5.02"
!IFNDEF _WIN32_IE
_WIN32_IE = 0x0600
!ENDIF # _WIN32_IE
!ENDIF # APPVER == 5.02

!IFNDEF _WIN32_IE
_WIN32_IE = 0x0400
!ENDIF

cc     = cl
link   = link
implib = lib
midl   = midl
rc     = Rc
hc     = Start /Wait Hcrtf
mc     = Mc
hcvars = -xn
sign   = SignTool
cab    = cabarc

# declarations common to all compiler options
ccommon = -c -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl -nologo -GS

!IFNDEF ansi
ccommon = $(ccommon) -D_UNICODE -DUNICODE
!ELSE
ccommon = $(ccommon) -DOLE2ANSI
!ENDIF

!IF "$(CPU)" == "i386"
cflags = $(ccommon) -D_X86_=1  -DWIN32 -D_WIN32 -W3
scall  = -Gz

!ELSEIF "$(CPU)" == "ALPHA"
cflags = $(ccommon) -D_ALPHA_=1  -DWIN32 -D_WIN32 -W3
scall  =

!ELSEIF "$(CPU)" == "IA64"
cflags = $(ccommon) -D_IA64_=1 -DWIN64 -D_WIN64  -DWIN32 -D_WIN32 -FIPRE64PRA.H
cflags = $(cflags) -Wp64 -W4
scall  =

!ELSEIF "$(CPU)" == "AMD64"
cflags = $(ccommon) -D_AMD64_=1 -DWIN64 -D_WIN64  -DWIN32 -D_WIN32 -FIPRE64PRA.H
cflags = $(cflags) -Wp64 -W4
scall  =

!ELSEIF "$(CPU)" == "ALPHA64"
cflags = $(ccommon) -D_AXP64_=1 -D_ALPHA64_=1 -DALPHA=1 -DWIN64 -D_WIN64 -DWIN32 -D_WIN32
cflags = $(cflags) -Wp64 -W4 -Ap64
scall  =
!ENDIF


!IF "$(APPVER)" == "4.0"
NMAKE_WINVER = 0x0400
!ELSEIF "$(APPVER)" == "5.0"
NMAKE_WINVER = 0x0500
!ELSEIF "$(APPVER)" == "5.01"
NMAKE_WINVER = 0x0501
!ELSEIF "$(APPVER)" == "5.02"
NMAKE_WINVER = 0x0502
!ENDIF

cflags = $(cflags) -D_WINNT -D_WIN32_WINNT=$(NMAKE_WINVER)

# regardless of the TARGET OS, define compile time WINVER to match APPVER macro
cflags = $(cflags) -D_WIN32_IE=$(_WIN32_IE) -DWINVER=$(NMAKE_WINVER)


# Set debugging options

!IF "$(CPU)" != "IA64"
!IFDEF NODEBUG
cdebug = -O1 -DNDEBUG
!ELSE IFDEF PROFILE
cdebug = -Gh -Zd -O1 -DNDEBUG
!ELSE IFDEF TUNE
cdebug = -Gh -Zd -O1 -DNDEBUG
!ELSE
cdebug = -Zi -Gm -Od -DDEBUG -RTC1
!ENDIF

!ELSE

!IFDEF NODEBUG
cdebug = -O1 -DNDEBUG
!ELSE
cdebug = -Zi -Gm -Od -DDEBUG -RTC1
!ENDIF

!ENDIF

!IFDEF NO_ANSI
noansi = -DNULL=0
!ENDIF

# for Windows applications that use the C Run-Time libraries
!IFDEF NODEBUG
cvarsmt    = $(noansi) -D_MT -MT
cvarsdll   = $(noansi) -D_MT -D_DLL -MD
#cvars      = $(noansi) $(cvarsmt)
cvars      = $(noansi) $(cvarsdll)
!ELSE
cvarsmt    = $(noansi) -D_MT -MTd
cvarsdll   = $(noansi) -D_MT -D_DLL -MDd
#cvars      = $(noansi) $(cvarsmt)
cvars      = $(noansi) $(cvarsdll)
!ENDIF

# resource compiler
rcflags = -r
!ifdef NODEBUG
rcvars    = -DWIN32 -D_WIN32 -DWINVER=$(NMAKE_WINVER) $(noansi)
MSVCRTLIB = CRT
DEBUGVER  = 0
!else
rcvars    = -DWIN32 -D_WIN32 -DWINVER=$(NMAKE_WINVER) -DDEBUG -D_DEBUG $(noansi)
MSVCRTLIB = DebugCRT
DEBUGVER  = 1
!endif

rcvars = $(rcvars) -I $(RESDIR) -I $(INCDIR) -I $(OUTDIR)

# -------------------------------------------------------------------------
# Platform Dependent MIDL Flags - must be specified after midl
#
#
# -------------------------------------------------------------------------
!IF "$(APPVER)" == "5.0"
MIDL_OPTIMIZATION=-target NT50
!ELSEIF "$(APPVER)" == "5.01"
MIDL_OPTIMIZATION=-target NT51
!ELSEIF "$(APPVER)" == "4.0"
MIDL_OPTIMIZATION=-target NT40
!ENDIF

!IF "$(CPU)" == "IA64"
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) -ia64
!ELSEIF "$(CPU)" == "AMD64"
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) -x64
!ELSE
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) -win32
!ENDIF

!IF ("$(APPVER)" != "4.0")
MIDL_OPTIMIZATION = $(MIDL_OPTIMIZATION) -robust
!ENDIF



# -------------------------------------------------------------------------
# Platform Dependent Link Flags - must be specified after $(link)
#
# Note: $(DLLENTRY) should be appended to each -entry: flag on the link
#       line.
#
# Note: When creating a DLL that uses C Run-Time functions it is
#       recommended to include the entry point function of the name DllMain
#       in the DLL's source code.  Also, the MAKEFILE should include the
#       -entry:_DllMainCRTStartup$(DLLENTRY) option for the creation of
#       this DLL.  (The C Run-Time entry point _DllMainCRTStartup in turn
#       calls the DLL defined DllMain entry point.)
#
# -------------------------------------------------------------------------

# declarations common to all linker options
lflags  = $(lflags) -INCREMENTAL:NO -NOLOGO

# declarations for use on Intel x86 systems
!IF "$(CPU)" == "i386"
DLLENTRY = @12
!ENDIF

# declarations for use on Compaq Alpha AXP systems
!IF "$(CPU)" == "ALPHA"
DLLENTRY =
!ENDIF

# declarations for use on Intel Architecture 64-bit systems
!IF "$(CPU)" == "IA64"
DLLENTRY =
!ENDIF

# declarations for use on AMD64 systems
!IF "$(CPU)" == "AMD64"
DLLENTRY =
!ENDIF

# declarations for use on Compaq 64-bit Alpha AXP systems
!IF "$(CPU)" == "ALPHA64"
DLLENTRY =
!ENDIF


# -------------------------------------------------------------------------
# Target Module Dependent Link Debug Flags - must be specified after $(link)
#
# These switches allow the inclusion of the necessary symbolic information
# for source level debugging with WinDebug, profiling and/or performance
# tuning.
#
# Note: Debug switches are on by default.
# -------------------------------------------------------------------------

!IF "$(CPU)" == "i386"

!IFDEF NODEBUG
ldebug = -RELEASE
!ELSE
ldebug = -DEBUG -DEBUGTYPE:cv
!ENDIF

!ELSE

!IFDEF NODEBUG
ldebug = -RELEASE
!ELSE IFDEF PROFILE
ldebug = -DEBUG:mapped,partial -DEBUGTYPE:coff
!ELSE IFDEF TUNE
ldebug = -DEBUG:mapped,partial -DEBUGTYPE:coff
!ELSE
ldebug = -DEBUG -DEBUGTYPE:cv
!ENDIF

!ENDIF


# -------------------------------------------------------------------------
# Subsystem Dependent Link Flags - must be specified after $(link)
#
# These switches allow for source level debugging with WinDebug for local
# and global variables.  They also provide the standard application type and
# entry point declarations.
#
# Note that on x86 screensavers have a WinMain entrypoint, but on RISC
# platforms it is main.  This is a Win95 compatibility issue.
#
# -------------------------------------------------------------------------

EXEVER = $(APPVER)

# ---------------------------------------------

# for Windows applications
conlflags = $(lflags) -subsystem:console,$(EXEVER) -machine:X86 
guilflags = $(lflags) -subsystem:windows,$(EXEVER) -machine:X86 
dlllflags = $(lflags) -entry:_DllMainCRTStartup$(DLLENTRY) -dll -machine:X86 

# For screen savers
!IF "$(CPU)" == "i386"
savlflags = $(lflags) -subsystem:windows,$(EXEVER) -machine:X86 -entry:WinMainCRTStartup
!ELSE
savlflags = $(lflags) -subsystem:windows,$(EXEVER) -machine:X86 -entry:mainCRTStartup
!ENDIF


# for compatibility with older-style makefiles
conflags  = $(conlflags)
guiflags  = $(guilflags)

# optional profiling and tuning libraries
!IF "$(CPU)" != "IA64"

!IFDEF PROFILE
optlibs =  cap.lib
!ELSE IFDEF TUNE
optlibs = wst.lib
!ELSE
optlibs =
!ENDIF

!ELSE
optlibs =
!ENDIF

winsocklibs = ws2_32.lib mswsock.lib

# basic subsystem specific libraries, less the C Run-Time
baselibs    = kernel32.lib $(optlibs) $(winsocklibs) advapi32.lib bufferoverflowu.lib version.lib
winlibs     = $(baselibs) user32.lib gdi32.lib comdlg32.lib winspool.lib

# for Windows applications that use the C Run-Time libraries
conlibs     = $(baselibs)
guilibs     = $(winlibs)

# for OLE applications
olelibs     = ole32.lib uuid.lib oleaut32.lib shell32.lib $(guilibs)

#for backwards compatibility
conlibsmt   = $(conlibs)
conlibsdll  = $(conlibs)
guilibsmt   = $(guilibs)
guilibsdll  = $(guilibs)
olelibsmt   = $(olelibs)
olelibsdll  = $(olelibs)

# for backward compatibility
ole2libs    = $(olelibs)
ole2libsmt  = $(olelibsmt)
ole2libsdll = $(olelibsdll)

# Set the Output Directory
!IF ("$(APPVER)" == "5.02") && ("$(CPU)" != "i386")

!IF "$(CPU)" == "IA64"
OUTDIR=SRV2003_IA64
!ELSEIF "$(CPU)" == "AMD64"
OUTDIR=x64
!ENDIF

!ELSE

!IF "$(APPVER)" == "5.0"
OUTDIR=WIN2000
!ELSEIF "$(APPVER)" == "5.01" 
OUTDIR=XP32
!ELSEIF "$(APPVER)" == "5.02"
OUTDIR=SRV2003
!ELSEIF "$(APPVER)" == "4.0"
OUTDIR=NT4
!ENDIF

!ENDIF

!IF "$(NODEBUG)" == ""
OUTDIR        = $(OUTDIR)_DEBUG
RESULTDIR     = $(PROJECTDIR)\debug
!ELSE
OUTDIR        = $(OUTDIR)_RETAIL
RESULTDIR     = $(PROJECTDIR)\release
!ENDIF

SLNDIR         = $(RESULTDIR)\sln
DISTDIR        = $(RESULTDIR)\dist
SIGNDIR        = $(RESULTDIR)\sign
MSIDIR         = $(RESULTDIR)\msi
MSIPATCHDIR    = $(RESULTDIR)\msipatch
DISTSRCDIR     = $(RESULTDIR)\src

HTMLDIR        = $(PROJECTDIR)\res\html
SCRIPTSDIR     = $(PROJECTDIR)\scripts
DOCDIR         = $(PROJECTDIR)\doc
MSITEMPLATEDIR = $(PROJECTDIR)\comp\msi\template
MSIPATCHTMPLDIR= $(PROJECTDIR)\comp\msi\patch

MSSDK          = C:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2


cinclude=-I$(INCDIR) -I$(OUTDIR)
cuseprecompheader=-Yu"os.h" -Fp"$(OUTDIR)\precomp.pch"
ccrtprecompheader=-Yc"os.h" -Fp"$(OUTDIR)\precomp.pch"
cvars= $(cvars) -EHa $(cinclude)

lvars=-MANIFEST:NO -LIBPATH:"$(MSSDK)\Lib" -LIBPATH:$(SLNDIR) -LIBPATH:$(PROJECTDIR)\lib\cab
#lvars=-MANIFEST -LIBPATH:$(SLNDIR) -LIBPATH:$(PROJECTDIR)\lib\cab

!ifdef clean
makeopts = clean=1
!endif # clean

!ifdef nodebug
makeopts = $(makeopts) nodebug=1
!endif

makeopts = $(makeopts) PROJECTDIR=$(PROJECTDIR)

BVR20983RES_RESULT=bvr20983.1.res.1
BVR20983CC_RESULT=bvr20983.1.cc.1
BVR20983SC_RESULT=bvr20983.1.sc.1
BVR20983UPDATE_RESULT=bvr20983.1.update.1
DIGICLOCK_RESULT=bvr20983.1.digiclock.1
BVR20983_RESULT=bvr20983.1.0.2
LSSTG_RESULT=bvr20983.1.lsstg.1
LSTYPEINFO_RESULT=bvr20983.1.lstypeinfo.1
MSICAB_RESULT=bvr20983.1.msicab.1
BVR20983MSGS_RESULT=bvr20983.1.msgs.1
BVR20983DESC=BVR2098 Suite
BVR20983_CABMSI=bvr20983.1.0.2.cab
BVR20983_MSI=bvr20983.1.0.2.msi
BVR20983_MSI_PACKAGECODE=BFE20983-0001-0101-0000-0002FFFFFFFF
BVR20983_MSI_COMPANY=Dorothea Wachmann

#ENDIF _MAKEFILE_INC_
!ENDIF
