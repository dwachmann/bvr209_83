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
!include <./inc/Makefile.inc>

!IF "$(NODEBUG)" == ""
RESULTDIR     = .\debug
!ELSE
RESULTDIR     = .\release
!ENDIF

SLNDIR        = $(RESULTDIR)\sln
DISTDIR       = $(RESULTDIR)\dist
SIGNDIR       = $(RESULTDIR)\sign
MSIDIR        = $(RESULTDIR)\msi

INCDIR        = .\inc
HTMLDIR       = .\res\html
SCRIPTSDIR    = .\scripts
SIGNPUBKEYTOK = 93425facf1ef717a
signvars      = sign /f $(signkey) /d "$(BVR20983DESC)" /du "https://bvr20983.berlios.de/" /t http://timestamp.verisign.com/scripts/timstamp.dll
patch         = cscript //nologo //job:patch $(SCRIPTSDIR)\patch.wsf /file:$(INCDIR)\ver\versions.xml /select:"/v:versions/" 

PROJECTS =          \
comp\libbvr20983\~  \
comp\res\~          \
comp\msgs\~         \
comp\cc\~           \
comp\sc\~           \
comp\digiclock\~    \
comp\lsstg\~        \
comp\lstypeinfo\~   \
comp\msicab\~       

CABCONTENT = \
$(INCDIR)\$(BVR20983_RESULT).inf \
$(SIGNDIR)\$(BVR20983MSGS_RESULT).dll \
$(SIGNDIR)\$(BVR20983SC_RESULT).dll \
$(SIGNDIR)\$(BVR20983CC_RESULT).dll \
$(SIGNDIR)\$(DIGICLOCK_RESULT).exe \
$(SIGNDIR)\$(LSSTG_RESULT).exe \
$(SIGNDIR)\$(LSTYPEINFO_RESULT).exe

CABRESULT = $(SIGNDIR)\$(BVR20983_RESULT).cab

!ifdef clean
all: $(PROJECTS) fullclean
!else
all: $(PROJECTS) 
!endif

distribute: $(PROJECTS) $(SIGNDIR) $(DISTDIR) $(CABRESULT)
  @copy $(HTMLDIR)\led.*                  $(DISTDIR)
  @copy $(HTMLDIR)\*.jpg                  $(DISTDIR)
  @copy $(SIGNDIR)\$(BVR20983_RESULT).cab $(DISTDIR)

patch:
  @$(patch) signpubkeytok $(SIGNPUBKEYTOK) crtlib $(MSVCRTLIB) debugver $(DEBUGVER)

$(CABRESULT): $(CABCONTENT)
	@$(cab) -s 6144 N $@ $**
	@$(sign) $(signvars) /p $(signpwd) $@

!include <./inc/Makefile.rule>
!include <./inc/bvr.inc>
