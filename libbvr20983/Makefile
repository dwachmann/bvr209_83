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

!include <../inc/Makefile.inc>

SRCS= \
$(SRCDIR)\sc\tlv.cpp \
$(SRCDIR)\sc\smartcard.cpp \
$(SRCDIR)\sc\scardeventstate.cpp \
$(SRCDIR)\sc\scardattribute.cpp \
$(SRCDIR)\sc\desedecipher.cpp \
$(SRCDIR)\sc\descipher.cpp \
$(SRCDIR)\sc\cardfile.cpp \
$(SRCDIR)\sc\bytebuffer.cpp \
$(SRCDIR)\sc\blockcipher.cpp \
$(SRCDIR)\sc\atr.cpp \
$(SRCDIR)\sc\apdu.cpp \
$(SRCDIR)\win\wndclass.cpp \
$(SRCDIR)\win\wnd.cpp \
$(SRCDIR)\win\tray.cpp \
$(SRCDIR)\win\ledwnd.cpp \
$(SRCDIR)\win\dialog.cpp \
$(SRCDIR)\win\dc.cpp \
$(SRCDIR)\win\colordialog.cpp \
$(SRCDIR)\win\clipboard.cpp \
$(SRCDIR)\win\app.cpp \
$(SRCDIR)\util\thread.cpp \
$(SRCDIR)\util\registry.cpp \
$(SRCDIR)\util\logstreambuf.cpp \
$(SRCDIR)\util\logstream.cpp \
$(SRCDIR)\util\loglevel.cpp \
$(SRCDIR)\util\logappender.cpp \
$(SRCDIR)\util\hr.cpp \
$(SRCDIR)\util\guid.cpp \
$(SRCDIR)\util\dllregisterserver.cpp \
$(SRCDIR)\util\comptrbase.cpp \
$(SRCDIR)\util\comlogstream.cpp \
$(SRCDIR)\util\apputil.cpp \
$(SRCDIR)\exception\smartcardexception.cpp \
$(SRCDIR)\exception\seexception.cpp \
$(SRCDIR)\exception\scardexception.cpp \
$(SRCDIR)\exception\scardapduexception.cpp \
$(SRCDIR)\exception\memoryexception.cpp \
$(SRCDIR)\exception\lasterrorexception.cpp \
$(SRCDIR)\exception\comexception.cpp \
$(SRCDIR)\exception\bvr20983exception.cpp \
$(SRCDIR)\com\covariant.cpp \
$(SRCDIR)\com\counknown.cpp \
$(SRCDIR)\com\cotypeinfo.cpp \
$(SRCDIR)\com\copropertyset.cpp \
$(SRCDIR)\com\copropertypage.cpp \
$(SRCDIR)\com\comserver.cpp \
$(SRCDIR)\com\collection.cpp \
$(SRCDIR)\com\coenumvariant.cpp \
$(SRCDIR)\com\coenuminst.cpp \
$(SRCDIR)\com\codispatch.cpp \
$(SRCDIR)\com\cocontrol.cpp \
$(SRCDIR)\com\coconnectionpoint.cpp \
$(SRCDIR)\com\cocollection.cpp \
$(SRCDIR)\com\coclassfactory.cpp

OBJS= \
$(OUTDIR)\os.obj \
$(OUTDIR)\tlv.obj \
$(OUTDIR)\smartcard.obj \
$(OUTDIR)\scardeventstate.obj \
$(OUTDIR)\scardattribute.obj \
$(OUTDIR)\desedecipher.obj \
$(OUTDIR)\descipher.obj \
$(OUTDIR)\cardfile.obj \
$(OUTDIR)\bytebuffer.obj \
$(OUTDIR)\blockcipher.obj \
$(OUTDIR)\atr.obj \
$(OUTDIR)\apdu.obj \
$(OUTDIR)\wndclass.obj \
$(OUTDIR)\wnd.obj \
$(OUTDIR)\tray.obj \
$(OUTDIR)\ledwnd.obj \
$(OUTDIR)\dialog.obj \
$(OUTDIR)\dc.obj \
$(OUTDIR)\colordialog.obj \
$(OUTDIR)\clipboard.obj \
$(OUTDIR)\app.obj \
$(OUTDIR)\thread.obj \
$(OUTDIR)\registry.obj \
$(OUTDIR)\logstreambuf.obj \
$(OUTDIR)\logstream.obj \
$(OUTDIR)\loglevel.obj \
$(OUTDIR)\logappender.obj \
$(OUTDIR)\hr.obj \
$(OUTDIR)\guid.obj \
$(OUTDIR)\dllregisterserver.obj \
$(OUTDIR)\comptrbase.obj \
$(OUTDIR)\comlogstream.obj \
$(OUTDIR)\apputil.obj \
$(OUTDIR)\smartcardexception.obj \
$(OUTDIR)\seexception.obj \
$(OUTDIR)\scardexception.obj \
$(OUTDIR)\scardapduexception.obj \
$(OUTDIR)\memoryexception.obj \
$(OUTDIR)\lasterrorexception.obj \
$(OUTDIR)\comexception.obj \
$(OUTDIR)\bvr20983exception.obj \
$(OUTDIR)\covariant.obj \
$(OUTDIR)\counknown.obj \
$(OUTDIR)\cotypeinfo.obj \
$(OUTDIR)\copropertyset.obj \
$(OUTDIR)\copropertypage.obj \
$(OUTDIR)\comserver.obj \
$(OUTDIR)\collection.obj \
$(OUTDIR)\coenumvariant.obj \
$(OUTDIR)\coenuminst.obj \
$(OUTDIR)\codispatch.obj \
$(OUTDIR)\cocontrol.obj \
$(OUTDIR)\coconnectionpoint.obj \
$(OUTDIR)\cocollection.obj \
$(OUTDIR)\coclassfactory.obj

# /Od /I "C:\projects\bvr20983\trunk\inc" /I "C:\projects\bvr20983\trunk\Debug" 
# /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_WIN32" /D "_WINNT" /D "CRTAPI1=_cdecl" /D "CRTAPI2=_cdecl" /D "_X86_=1" /D "_MT" /D "_UNICODE" /D "UNICODE" 
# /Gm /EHsc /RTC1 /MTd /Yu"os.h" /Fp"Debug\libbvr20983.pch" /Fo"Debug\\" /Fd"Debug\vc80.pdb" /W3 /c /ZI /TP
# /Gm /EHsc /RTC1 /MTd /Yc"os.h" /Fp"Debug\libbvr20983.pch" /Fo"Debug\\" /Fd"Debug\vc80.pdb" /W3 /c /ZI /TP ..\src\os.cpp

all: $(OUTDIR) $(OUTDIR)\libbvr20983.lib

$(OUTDIR)\libbvr20983.lib: $(OBJS)
  $(implib) /machine:$(CPU) /out:$@ $**

!include <../inc/Makefile.rule>
