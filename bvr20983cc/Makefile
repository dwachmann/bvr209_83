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
$(SRCDIR)\com\ocx\coled.cpp \
$(SRCDIR)\bvr20983cc.cpp \
$(OUTDIR)\ibvr20983ccguid.c

OBJS= \
$(OUTDIR)\os.obj \
$(OUTDIR)\bvr20983cc.obj \
$(OUTDIR)\coled.obj \
$(OUTDIR)\ibvr20983ccguid.obj \
$(OUTDIR)\bvr20983cc.res

all: $(OUTDIR) $(OUTDIR)\bvr20983cc.dll

$(SRCDIR)\bvr20983cc.cpp: $(OUTDIR)\ibvr20983cc.h

$(OUTDIR)\ibvr20983cc.h $(OUTDIR)\ibvr20983ccguid.c $(OUTDIR)\ibvr20983cc-0000.tlb: $(INCDIR)\com\ibvr20983cc-0000.odl
  $(midl) $(MIDL_OPTIMIZATION) /out $(OUTDIR) /I $(INCDIR) /char signed /tlb ibvr20983cc-0000.tlb /h ibvr20983cc.h /iid ibvr20983ccguid.c $**

$(OUTDIR)\bvr20983cc.res: $(RESDIR)\bvr20983cc.rc $(OUTDIR)\ibvr20983cc-0000.tlb
  $(rc) $(rcflags) $(rcvars) /fo $@ $(RESDIR)\bvr20983cc.rc

$(OUTDIR)\bvr20983cc.dll: $(OBJS) $(INCDIR)\bvr20983cc.def
  $(link) $(ldebug) $(dlllflags) $(lvars) -def:$(INCDIR)\bvr20983cc.def -out:$@ -pdb:$(@R).pdb $(olelibsmt) libbvr20983.lib $(OBJS)

!include <../inc/Makefile.rule>
