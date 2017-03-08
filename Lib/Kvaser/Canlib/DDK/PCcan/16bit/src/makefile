##
##                        Copyright 1998 by KVASER AB            
##                  P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
##            E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
##
## This software is furnished under a license and may be used and copied
## only in accordance with the terms of such license.
##
#############################################################################
#
!include setup.mak

default: all

all everything clean lint depend:
    verstamp -noupdate version.hx version.dat version.bld version.h
    verstamp -noupdate ver31.rcx version.dat version.bld ver31.rc
    verstamp version.rcx version.dat version.bld version.rc
    $(MAKE) -$(MAKEFLAGS) PLATFORM=DOS COMP=DEFAULT_BCC_16 BUILD_DIR=out\bccdos.ndb -f bcc.mak $*
    $(MAKE) -$(MAKEFLAGS) PLATFORM=WIN16 COMP=DEFAULT_BCC_16 BUILD_DIR=out\bccw16.ndb -f bcc.mak $*
    $(MAKE) -$(MAKEFLAGS) DEBUG=1 PLATFORM=DOS COMP=DEFAULT_BCC_16 BUILD_DIR=out\bccdos.db -f bcc.mak $*
    $(MAKE) -$(MAKEFLAGS) DEBUG=1 PLATFORM=WIN16 COMP=DEFAULT_BCC_16 BUILD_DIR=out\bccw16.db -f bcc.mak $*
    $(MAKE) -$(MAKEFLAGS) PLATFORM=DOS COMP=DEFAULT_MSC_16 BUILD_DIR=out\mscdos.ndb -f msc.mak $*
    $(MAKE) -$(MAKEFLAGS) PLATFORM=WIN16 COMP=DEFAULT_MSC_16 BUILD_DIR=out\mscw16.ndb -f msc.mak $*
    $(MAKE) -$(MAKEFLAGS) DEBUG=1 PLATFORM=DOS COMP=DEFAULT_MSC_16 BUILD_DIR=out\mscdos.db -f msc.mak $*
    $(MAKE) -$(MAKEFLAGS) DEBUG=1 PLATFORM=WIN16 COMP=DEFAULT_MSC_16 BUILD_DIR=out\mscw16.db -f msc.mak $*


install:
   $(INSTALL) out\mscw16.ndb\canlib.dll  $(GLOBAL_SYS_DIR)
   $(INSTALL) out\mscw16.ndb\canlib.lib  $(GLOBAL_LIB_DIR_MSC)
   $(INSTALL) out\mscdos.ndb\canl.lib    $(GLOBAL_LIB_DIR_MSC)
   $(INSTALL) out\bccw16.ndb\canlib.lib  $(GLOBAL_LIB_DIR_BCC)
   $(INSTALL) out\bccdos.ndb\canl.lib    $(GLOBAL_LIB_DIR_BCC)


release:
   $(INSTALL) out\mscw16.ndb\canlib.dll     $(RELEASE_DIR_SYS)
   $(INSTALL) out\mscw16.ndb\canlib31.dll   $(RELEASE_DIR_SYS)\Win3\canlib.dll
   $(INSTALL) 00readme-win3x.txt            $(RELEASE_DIR_SYS)\Win3\00readme.txt
   $(INSTALL) out\mscdos.ndb\canl.lib       $(RELEASE_DIR_LIB_MSC)
   $(INSTALL) out\mscw16.ndb\canlib.lib     $(RELEASE_DIR_LIB_MSC)
   $(INSTALL) out\bccdos.ndb\canl.lib       $(RELEASE_DIR_LIB_BCC)
   $(INSTALL) out\bccw16.ndb\canlib.lib     $(RELEASE_DIR_LIB_BCC)


setup:
    -$(MKDIR) out\bccdos.ndb
    -$(MKDIR) out\bccw16.ndb
    -$(MKDIR) out\bccdos.db
    -$(MKDIR) out\bccw16.db
    -$(MKDIR) out\mscdos.ndb
    -$(MKDIR) out\mscw16.ndb
    -$(MKDIR) out\mscdos.db
    -$(MKDIR) out\mscw16.db
    -$(MKDIR) $(RELEASE_DIR_SYS)\Win3

