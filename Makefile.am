AM_CXXFLAGS=-Wall

AM_CXXFLAGS += -D_REENTRANT -DQT_WEBKIT -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED

bin_PROGRAMS = ji3
ji3_SOURCES = \
		src/main.cpp \
		src/SoundManager.cpp \
		src/Logger.cpp \
		src/Util.cpp \
		src/User.cpp \
		src/TestResult.cpp \
		src/ScenarioManager.cpp \
		src/Viewer.cpp \
		src/ControlPanel.cpp \
		src/Graph.cpp \
		src/GraphPanel.cpp \
		src/ResultTable.cpp \
		src/moc_ControlPanel.cpp \
		src/moc_Graph.cpp \
		src/moc_Viewer.cpp \
		src/moc_GraphPanel.cpp

src/moc_Viewer.cpp: src/Viewer.hpp
	moc $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $< -o $@

src/moc_Graph.cpp: src/Graph.hpp
	moc $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $< -o $@

src/moc_ControlPanel.cpp: src/ControlPanel.hpp
	moc $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $< -o $@

src/moc_GraphPanel.cpp: src/GraphPanel.hpp
	moc $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $< -o $@

ji3_CPPFLAGS = @QTCORE_CFLAGS@ @QTGUI_CFLAGS@ @SDL_CFLAGS@
ji3_LDADD = @QTCORE_LIBS@ @QTGUI_LIBS@ @SDL_LIBS@

if WINDOWS
ji3_LDADD += -lws2_32 -lgdi32
else
ji3_LDADD += -ldl -lpthread
endif

SUBDIRS = test

# Windows installer
if WINDOWS
all-local: @PACKAGE_TARNAME@-@VERSION@-setup$(EXEEXT)

@PACKAGE_TARNAME@-@VERSION@-setup$(EXEEXT): ji3.iss ji3$(EXEEXT)
	@STRIP@ ji3$(EXEEXT)
	$(WINE) "@ISCC@" /q /dMyAppName=@PACKAGE_NAME@ /dMyAppVersion=@PACKAGE_VERSION@ /dMyAppExeName=ji3$(EXEEXT) /f@PACKAGE_TARNAME@-@VERSION@-setup $<
endif

clean-local:
	$(RM) @PACKAGE_TARNAME@-@VERSION@-setup$(EXEEXT)
	$(RM) src/moc_*.cpp

