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

OBJS= \
$(OUTDIR)\os.obj \
$(OUTDIR)\cowallet.obj \
$(OUTDIR)\cosmartcard.obj \
$(OUTDIR)\coatr.obj \
$(OUTDIR)\bvr20983.obj \
$(OUTDIR)\bvr20983guid.obj \
$(OUTDIR)\bvr20983.res

!ifdef clean
all: clean
!else
all: $(OUTDIR) $(SLNDIR) $(OUTDIR)\ibvr20983-0007.tlb $(OUTDIR)\ibvr20983-0000.tlb $(SLNDIR)\bvr20983.dll
!endif

$(OUTDIR)\ibvr20983-0007.tlb: $(INCDIR)\com\ibvr20983-0007.odl
  $(midl) $(MIDL_OPTIMIZATION) /out $(OUTDIR) /I $(INCDIR) /char signed /tlb ibvr20983-0007.tlb $**

$(OUTDIR)\ibvr20983.h $(OUTDIR)\bvr20983guid.c $(OUTDIR)\ibvr20983-0000.tlb: $(INCDIR)\com\ibvr20983-0000.odl
  $(midl) $(MIDL_OPTIMIZATION) /out $(OUTDIR) /I $(INCDIR) /char signed /tlb ibvr20983-0000.tlb /h ibvr20983.h /iid bvr20983guid.c $**

$(OUTDIR)\bvr20983.res: $(RESDIR)\bvr20983.rc $(OUTDIR)\ibvr20983-0000.tlb
  $(rc) $(rcflags) $(rcvars) /fo $@ $(RESDIR)\bvr20983.rc

$(OUTDIR)\bvr20983.dll: $(OBJS) $(INCDIR)\bvr20983.def
  $(link) $(ldebug) $(dlllflags) $(lvars) -def:$(INCDIR)\bvr20983.def -out:$@ -pdb:$(@R).pdb $(olelibsmt) winscard.lib libbvr20983.lib $(OBJS)

$(SLNDIR)\bvr20983.dll $(SLNDIR)\bvr20983.lib: $(OUTDIR)\bvr20983.dll
  @copy $(OUTDIR)\bvr20983.dll $(SLNDIR)
  @copy $(OUTDIR)\bvr20983.lib $(SLNDIR)

clean:
  if exist $(OUTDIR)\$(NULL) rd /s /q $(OUTDIR)
  if exist $(SLNDIR)\bvr20983.dll del /q $(SLNDIR)\bvr20983.dll
  if exist $(SLNDIR)\bvr20983.lib del /q $(SLNDIR)\bvr20983.lib

!include <../inc/Makefile.rule>
