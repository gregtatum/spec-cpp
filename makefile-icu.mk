# Copyright (C) 2016 and later: Unicode, Inc. and others.
# License & terms of use: http://www.unicode.org/copyright.html
## -*-makefile-*-
#******************************************************************************
#   Copyright (C) 1999-2014, International Business Machines
#   Corporation and others.  All Rights Reserved.
#******************************************************************************
# This Makefile.inc is designed to be included into projects which make use
# of the ICU.

# CONTENTS OF THIS FILE
# 1). Base configuration information and linkage
# 2). Variables giving access to ICU tools
# 3). Host information
# 4). Compiler flags and settings
# 5). Data Packaging directives
# 6). Include of platform make fragment (mh-* file)

##################################################################
##################################################################
#
#       *1*  base configuration information and linkage
#
##################################################################
# The PREFIX is the base of where ICU is installed.
# Inside this directory you should find  bin, lib, include/unicode,
# etc.  If ICU is not installed in this directory, you must change the
# following line.  There should exist $(prefix)/include/unicode/utypes.h
# for example.
prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
bindir = ${exec_prefix}/bin
datarootdir = ${prefix}/share
datadir = ${datarootdir}
sbindir = ${exec_prefix}/sbin

# about the ICU version
VERSION = 68.2
UNICODE_VERSION = 13.0

# The prefix for ICU libraries, normally 'icu'
ICUPREFIX = icu
PACKAGE = icu
LIBICU = lib$(ICUPREFIX)

# Static library prefix and file extension
STATIC_PREFIX = s
LIBSICU = lib$(STATIC_PREFIX)$(ICUPREFIX)
A = a

# Suffix at the end of libraries. Usually empty.
ICULIBSUFFIX =
# ICULIBSUFFIX_VERSION  is non-empty if it is to contain a library
# version. For example, if it is 21, it means libraries are named
# libicuuc21.so for example.

# rpath links a library search path right into the binaries.
## mh-files MUST NOT override RPATHLDFLAGS unless they provide
## equivalent '#SH#' lines for icu-config fixup
ENABLE_RPATH = NO
ifeq ($(ENABLE_RPATH),YES)
RPATHLDFLAGS = $(LD_RPATH)$(LD_RPATH_PRE)$(libdir)
endif

#SH## icu-config version of above 'if':
#SH#case "x$ENABLE_RPATH" in
#SH#     x[yY]*)
#SH# 	ENABLE_RPATH=YES
#SH# 	RPATHLDFLAGS="${LD_RPATH}${LD_RPATH_PRE}${libdir}"
#SH# 	;;
#SH#
#SH#     x[nN]*)
#SH# 	ENABLE_RPATH=NO
#SH# 	RPATHLDFLAGS=""
#SH# 	;;
#SH#
#SH#     x)
#SH# 	ENABLE_RPATH=NO
#SH# 	RPATHLDFLAGS=""
#SH# 	;;
#SH#
#SH#     *)
#SH# 	echo $0: Unknown --enable-rpath value ${ENABLE_RPATH} 1>&2
#SH# 	exit 3
#SH# 	;;
#SH#esac

# Name flexibility for the library naming scheme.  Any modifications should
# be made in the mh- file for the specific platform.
DATA_STUBNAME = data
COMMON_STUBNAME = uc
I18N_STUBNAME = i18n
LAYOUTEX_STUBNAME = lx
IO_STUBNAME = io
TOOLUTIL_STUBNAME = tu
CTESTFW_STUBNAME = test



### To link your application with ICU:
# 1. use LDFLAGS, CFLAGS, etc from above
# 2. link with $(ICULIBS)
# 3. optionally, add one or more of:
#    - $(ICULIBS_I18N)    - i18n library, formatting, etc.
#    - $(ICULIBS_ICUIO)   - ICU stdio equivalent library

ICULIBS_COMMON = -l$(ICUPREFIX)uc$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_DATA   = -l$(ICUPREFIX)$(DATA_STUBNAME)$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_I18N   = -l$(ICUPREFIX)$(I18N_STUBNAME)$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_TOOLUTIL = -l$(ICUPREFIX)tu$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_CTESTFW = -l$(ICUPREFIX)ctestfw$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_ICUIO = -l$(ICUPREFIX)io$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_OBSOLETE = -l$(ICUPREFIX)obsolete$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_LAYOUTEX = -l$(ICUPREFIX)lx$(ICULIBSUFFIX)$(ICULIBSUFFIX_VERSION)
ICULIBS_BASE = -L$(libdir)

# for icu-config to test with
ICULIBS_COMMON_LIB_NAME = ${LIBICU}${COMMON_STUBNAME}${ICULIBSUFFIX}${ICULIBSUFFIX_VERSION}.${SO}
ICULIBS_COMMON_LIB_NAME_A = ${LIBICU}${COMMON_STUBNAME}${ICULIBSUFFIX}.${A}

# ICULIBS is the set of libraries your application should link
# with usually. Many applications will want to add $(ICULIBS_I18N) as well.
ICULIBS = $(ICULIBS_BASE) $(ICULIBS_I18N) $(ICULIBS_COMMON) $(ICULIBS_DATA)
