AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libmvtxonline2019.la

#ROOTCINT =  $(ROOTSYS)/bin/rootcint
ROOTCINT =  rootcint

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(ONLINE_MAIN)/include \
  -I@ROOTINC@

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ROOTSYS)/lib \
  -L$(ONLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib

libmvtxonline2019_la_LIBADD = \
  -lpmonitor \
  -lEvent -lNoRootEvent -lmessage

pkginclude_HEADERS = \
  mvtxOnlineQA.h 

libmvtxonline2019_la_SOURCES = \
  mvtxOnlineQA.cc 

################################################
# linking tests

BUILT_SOURCES = testexternals.C

noinst_PROGRAMS = \
  testexternals_tracker

testexternals_tracker_SOURCES = testexternals.C
testexternals_tracker_LDADD = libmvtxonline2019.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f $(BUILT_SOURCES)
