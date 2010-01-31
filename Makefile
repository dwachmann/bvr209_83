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

!ifdef clean
all: $(PROJECTS) fullclean
!else
all: $(PROJECTS) 
!endif

rebuildall: incbuild patch $(PROJECTS) 

projects: $(PROJECTS) 

distribute: $(PROJECTS) comp\msi\~msidistribute

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

comp\msi\~msidistribute:
  @IF EXIST $(@D)\makefile <<nmaketmp.bat
  @cd $(@D)
  @$(MAKE) -nologo /$(MAKEFLAGS) $(makeopts) signkey=$(signkey) signpwd=$(signpwd) msidistribute
<<

comp\msi\~msitransform:
  @IF EXIST $(@D)\makefile <<nmaketmp.bat
  @cd $(@D)
  @$(MAKE) -nologo /$(MAKEFLAGS) $(makeopts) signkey=$(signkey) signpwd=$(signpwd) msitransform
<<

!include <./inc/Makefile.rule>
!include <./inc/bvr.inc>
