APP=wndmenu
EXE=$(APP).exe
OUTDIR=build
SRCS=src\main.cpp src\gui.cpp src\control.cpp

CXX=cl
CXXFLAGS=/nologo /std:c++17 /O2 /GL /GF /Gy /fp:fast /Zc:inline /GR- /EHsc /DNDEBUG /DUNICODE /D_UNICODE
#LDFLAGS=/nologo /LTCG /OPT:REF /OPT:ICF /SUBSYSTEM:WINDOWS user32.lib gdi32.lib
LDFLAGS=/nologo /LTCG /OPT:REF /OPT:ICF user32.lib gdi32.lib shell32.lib

all: $(OUTDIR)\$(EXE)

$(OUTDIR):
	@if not exist $(OUTDIR) mkdir $(OUTDIR)

$(OUTDIR)\$(EXE): $(SRCS) $(OUTDIR)
	$(CXX) $(CXXFLAGS) /Fe$@ $(SRCS) /link $(LDFLAGS)

clean:
	@if exist $(OUTDIR) rmdir /s /q $(OUTDIR)

# Optional per-user install (no admin)
PREFIX=$(LOCALAPPDATA)\wndmenu

install: all
	@if not exist "$(PREFIX)" mkdir "$(PREFIX)"
	copy /y "$(OUTDIR)\$(EXE)" "$(PREFIX)\$(EXE)"
	reg add "HKCU\Software\Microsoft\Windows\CurrentVersion\App Paths\$(EXE)" /ve /t REG_SZ /d "$(PREFIX)\$(EXE)" /f
	reg add "HKCU\Software\Microsoft\Windows\CurrentVersion\App Paths\$(EXE)" /v Path /t REG_SZ /d "$(PREFIX)" /f

uninstall:
	reg delete "HKCU\Software\Microsoft\Windows\CurrentVersion\App Paths\$(EXE)" /f
	@if exist "$(PREFIX)\$(EXE)" del /q "$(PREFIX)\$(EXE)"
