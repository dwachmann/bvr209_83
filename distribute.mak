SIGNTOOL=SignTool
SIGNTOOL_PARAMS=sign /f $(SIGNDIR)/bvr20983-01.pfx /d "BVR20983 Security Suite" /du "https://www.jondos.de/" /t http://timestamp.verisign.com/scripts/timstamp.dll 
CAB=cabarc

DISTDIR=.\dist
BUILDDIR=.\debug
SIGNDIR=.\sign
HTMLDIR=.\html

!IFDEF NODEBUG
BUILDDIR=.\release
!ENDIF

.SUFFIXES: .dll .exe

all: prepare sign $(DISTDIR)\bvr20983.cab
	@copy $(DISTDIR)\bvr20983.cab $(HTMLDIR)

clean:
  -rmdir /s /q $(DISTDIR)

# Make the temporary work sub-directory.
prepare:
	@if not exist $(DISTDIR) mkdir $(DISTDIR)
	@if exist $(DISTDIR)\bvr20983.cab del $(DISTDIR)\bvr20983.cab

{$(BUILDDIR)}.dll{$(DISTDIR)}.dll:
	@$(SIGNTOOL) $(SIGNTOOL_PARAMS) /p $(PWD) $<
	@copy $< $@

{$(BUILDDIR)}.exe{$(DISTDIR)}.exe:
	@$(SIGNTOOL) $(SIGNTOOL_PARAMS) /p $(PWD) $<
	@copy $< $@

sign: $(DISTDIR)\bvr20983.dll $(DISTDIR)\bvr20983cc.dll $(DISTDIR)\scc.exe $(DISTDIR)\digiclock.exe

$(DISTDIR)\bvr20983.cab: $(DISTDIR)\bvr20983.dll $(DISTDIR)\bvr20983cc.dll $(SIGNDIR)\bvr20983.inf
	@$(CAB) -s 6144 N $@ $?
	@$(SIGNTOOL) $(SIGNTOOL_PARAMS) /p $(PWD) $@
