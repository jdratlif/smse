AUTOMAKE_OPTIONS = subdir-objects
CLEANFILES = res/smse.cc
EXTRA_DIST = res/icon32x32.ico res/icon32x32.xpm res/Info.plist res/smse.icns \
	res/smse.xrc res/windows.rc

bin_PROGRAMS = smse

smse_SOURCES = AppConstants.hh \
	App.cc App.hh \
	exceptions/FileIOException.cc exceptions/FileIOException.hh \
	exceptions/InvalidButtonException.cc \
	exceptions/InvalidButtonException.hh \
	exceptions/InvalidSRAMDataException.cc \
	exceptions/InvalidSRAMDataException.hh \
	exceptions/InvalidSRAMFileException.cc \
	exceptions/InvalidSRAMFileException.hh \
	model/SRAMFile.cc model/SRAMFile.hh \
	res/smse.cc \
	view/Frame.cc view/Frame.hh \
	view/SRAMFileTarget.cc view/SRAMFileTarget.hh

if WINDOWS
smse_SOURCES += res/windows.rc
endif

.rc.o:
	windres @WX_INCLUDES@ -o $@ $<

.xrc.cc:
	wxrc -c -o $@ $<

smse.app: smse res/smse.icns res/Info.plist
	-mkdir smse.app
	-mkdir smse.app/Contents
	-mkdir smse.app/Contents/MacOS
	-mkdir smse.app/Contents/Resources
	cp smse smse.app/Contents/MacOS
	cp res/Info.plist smse.app/Contents
	cp res/smse.icns smse.app/Contents/Resources
	echo -n 'APPL????' > smse.app/Contents/PkgInfo
