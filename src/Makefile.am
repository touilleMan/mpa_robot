SUBDIRS = robot

AM_CPPFLAGS = -I$(top_srcdir)/src/robot

bin_PROGRAMS = mpa_emulator
mpa_emulator_SOURCES = main.cc emulator.cc emulator.hh
mpa_emulator_LDADD = robot/librobot.la
