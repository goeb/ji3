
WIN32_COMPONENTS = DLL ji3.bat Data Couleurs LICENSE README.md TestNormal
SRC_COMPONENTS = ji3.bat src Makefile ji3.pro Data Couleurs LICENSE README.md TestNormal 

pkg: pkg-win32 pkg-src pkg-other


WIN_PKG_DIR = ji3-win32
SRC_PKG_DIR = ji3-src
pkg-win32:
	rm -rf $(WIN_PKG_DIR)
	mkdir -p $(WIN_PKG_DIR)
	cp -f win32/ji3.exe DLL/.
	for c in $(WIN32_COMPONENTS); do ln -fs ../$$c $(WIN_PKG_DIR)/$$c; done
	zip -r $(WIN_PKG_DIR).zip $(WIN_PKG_DIR)

pkg-src:
	rm -rf $(SRC_PKG_DIR)
	mkdir -p $(SRC_PKG_DIR)
	for c in $(SRC_COMPONENTS); do ln -fs ../$$c $(SRC_PKG_DIR)/$$c; done
	zip -r $(SRC_PKG_DIR).zip $(SRC_PKG_DIR)

pkg-other:
	zip -r ji3-other.zip Instruments Animaux
