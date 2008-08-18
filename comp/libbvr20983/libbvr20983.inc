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
$(OUTDIR)\registryutil.obj \
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
$(OUTDIR)\dllgetclass.obj \
$(OUTDIR)\coclassfactory.obj \
$(OUTDIR)\versioninfo.obj

!ifdef clean
all: clean
!else
all: $(OUTDIR) $(SLNDIR) $(SLNDIR)\$(LIBBVR20983).lib
!endif

$(SLNDIR)\$(LIBBVR20983).lib: $(OBJS)
  $(implib) /machine:$(CPU) /out:$@ $**

clean:
  if exist $(OUTDIR)\$(NULL) rd /s /q $(OUTDIR)
  if exist $(SLNDIR)\$(LIBBVR20983).lib del /q $(SLNDIR)\$(LIBBVR20983).lib
