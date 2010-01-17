# $Id$
# 
# Copyright (C) 2009 Dorothea Wachmann
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
# 18

PROJECTDIR    = $(MAKEDIR)

!include <./inc/Makefile.inc>

signvars = sign /f $(signkey) /d "$(BVR20983DESC)" /du "https://bvr20983.berlios.de/" /t http://timestamp.verisign.com/scripts/timstamp.dll

PROJECTS =          \
comp\libbvr20983\~  \
comp\res\~          \
comp\msgs\~         \
comp\update\~       \
comp\cc\~           \
comp\sc\~           \
comp\digiclock\~    \
comp\lsstg\~        \
comp\lstypeinfo\~   \
comp\msicab\~       \
comp\msi\~       

CABCONTENT = \
#$(INCDIR)\$(BVR20983_RESULT).inf \
$(SIGNDIR)\$(BVR20983MSGS_RESULT).dll \
$(SIGNDIR)\$(BVR20983UPDATE_RESULT).dll \
$(SIGNDIR)\$(BVR20983SC_RESULT).dll \
$(SIGNDIR)\$(BVR20983CC_RESULT).dll \
$(SIGNDIR)\$(DIGICLOCK_RESULT).exe \
$(SIGNDIR)\$(LSSTG_RESULT).exe \
$(SIGNDIR)\$(BVR20983_MSI) \
$(SIGNDIR)\$(LSTYPEINFO_RESULT).exe

CABRESULT = $(SIGNDIR)\$(BVR20983_RESULT).cab

!ifdef clean
all: $(PROJECTS) fullclean
!else
all: $(PROJECTS) 
!endif

rebuildall: incbuild patch $(PROJECTS) 

projects: $(PROJECTS) 

distribute: $(PROJECTS) $(SIGNDIR) $(DISTDIR) $(CABRESULT)
  @copy $(HTMLDIR)\led.*                  $(DISTDIR)
  @copy $(HTMLDIR)\*.jpg                  $(DISTDIR)
  @copy $(SIGNDIR)\$(BVR20983_RESULT).cab $(DISTDIR)

patch:
  cscript //nologo //job:patch $(SCRIPTSDIR)\patch.wsf /file:$(INCDIR)\ver\versions.xml /select:"/v:versions//v:patch" 

patch1:
  cscript //nologo //job:patch $(SCRIPTSDIR)\patch.wsf /file:$(INCDIR)\ver\versions.xml /select:"/v:versions//v:patch[@filename='bvr20983volatile-ver.h']" 

incbuild:
  cscript //nologo //job:incbuild $(SCRIPTSDIR)\patch.wsf /file:$(INCDIR)\ver\versions.xml

tagbuild:
  cscript //nologo //job:tagbuild $(SCRIPTSDIR)\patch.wsf /file:$(INCDIR)\ver\versions.xml

tagmsi:
  cscript //nologo //job:tagmsiversion $(SCRIPTSDIR)\patch.wsf /file:$(INCDIR)\ver\versions.xml
  
msicab: comp\msi\~msicab

msipatch: comp\msi\~createpatch

msitransform: comp\msi\~msitransform

comp\msi\~createpatch:
  @IF EXIST $(@D)\makefile <<nmaketmp.bat
  @cd $(@D)
  @$(MAKE) -nologo /$(MAKEFLAGS) $(makeopts) createpatch
<<

comp\msi\~msicab:
  @IF EXIST $(@D)\makefile <<nmaketmp.bat
  @cd $(@D)
  @$(MAKE) -nologo /$(MAKEFLAGS) $(makeopts) msicab
<<

comp\msi\~msitransform:
  @IF EXIST $(@D)\makefile <<nmaketmp.bat
  @cd $(@D)
  @$(MAKE) -nologo /$(MAKEFLAGS) $(makeopts) msitransform
<<

$(CABRESULT): $(CABCONTENT)
	@$(cab) -s 6144 N $@ $**
	@$(sign) $(signvars) /p $(signpwd) $@

!include <./inc/Makefile.rule>
!include <./inc/bvr.inc>
