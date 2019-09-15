SUBDIRS = source
EXTRA_DIST = apidocs docs/*.txt Doxyfile srm \
	vstudio/smse.sln vstudio/smse.vcxproj \
	vstudio/smse.vcxproj.filters vstudio/smse.vcxproj.user

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
