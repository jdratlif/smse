# $Id: Makefile.am,v 1.4 2008/12/17 04:04:17 jdratlif Exp $

SUBDIRS = source
EXTRA_DIST = apidocs docs/*.txt Doxyfile srm \
	vstudio/smse/smse.sln vstudio/smse/smse.vcproj

srm:
	-mkdir srm
	-cp source/res/Super\ Metroid\ \(JU\)\ \[\!\].srm srm

.PHONY: allclean apidocs bundle touch

allclean: clean
	-rm -rf source/smse.app
	-rm -rf apidocs
	-rm -f *.bz2
	-find -iname "*~" -exec rm -f {} \;

apidocs: Doxyfile
	@doxygen

bundle:
	-cd source && make smse.app

touch:
	-find -iname "*.cc" -exec touch {} \;
	-find -iname "*.hh" -exec touch {} \;
	-find -iname "*.xrc" -exec touch {} \;

