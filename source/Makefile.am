AUTOMAKE_OPTIONS = subdir-objects
CLEANFILES = res/smse.cc
EXTRA_DIST = res/icon32x32.ico res/icon32x32.xpm res/smse.xrc res/windows.rc

bin_PROGRAMS = smse

smse_SOURCES = AppConstants.hh \
    App.cc App.hh \
    exceptions/FileIOException.cc exceptions/FileIOException.hh \
    exceptions/InvalidButtonException.cc exceptions/InvalidButtonException.hh \
    exceptions/InvalidSRAMDataException.cc exceptions/InvalidSRAMDataException.hh \
    exceptions/InvalidSRAMFileException.cc exceptions/InvalidSRAMFileException.hh \
    model/SRAMFile.cc model/SRAMFile.hh \
    view/Frame.cc view/Frame.hh

nodist_smse_SOURCES = res/smse.cc

if WINDOWS
smse_SOURCES += res/windows.rc
endif

.rc.o:
	windres @WX_INCLUDES@ -o $@ $<

.xrc.cc:
	wxrc -c -o $@ $<

