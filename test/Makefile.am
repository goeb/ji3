
TESTS = test_Util

check_PROGRAMS = test_Util
test_Util_SOURCES = test_Util.cpp ../src/Util.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src

# include the tests on the distribution
EXTRA_DIST = .

# cleanup tests results before including
dist-hook:
	rm -f $(distdir)/.git* $(distdir)/*~

clean-local:
	rm -f *.out
