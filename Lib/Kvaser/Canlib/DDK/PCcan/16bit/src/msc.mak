##
##                        Copyright 1998 by KVASER AB            
##                  P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
##            E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
##
## This software is furnished under a license and may be used and copied
## only in accordance with the terms of such license.
##
##  $Revision$
##  $Header$
#############################################################################
#
!INCLUDE setup.mak

SRCS= canlib.c global.c irq.c libcanc.c libpccan.c misc.c timer.c 

OBJS= \
+$(BUILD_DIR)\canlib.obj\
+$(BUILD_DIR)\global.obj\
+$(BUILD_DIR)\irq.obj\
+$(BUILD_DIR)\libcanc.obj\
+$(BUILD_DIR)\libpccan.obj\
+$(BUILD_DIR)\misc.obj\
+$(BUILD_DIR)\timer.obj 

# Billigt. Fixa en makedepend.
INCS= 82c200.h canc.h cancard.h getargs.h global.h i82527.h irq.h \
      libcanc.h pccan.h std.h misc.h version.h timer.h

OBJFILES= $(OBJS:+= )

CFLAGS=$(CFLAGS) -I$(GLOBAL_INCLUDE_DIR)

#
# Define our target
#
!if "$(PLATFORM)" == "DOS"

all: $(BUILD_DIR)\canl.lib
CFLAGS=$(CFLAGS) $(CFLAGS_EXE)

!elseif "$(PLATFORM)" == "WIN16"

all: $(BUILD_DIR)\canlib.dll
CFLAGS=$(CFLAGS) $(CFLAGS_DLL)

!else
!error PLATFORM is not correctly defined; should be DOS or WIN16
!endif

#
# Target canl.lib is for DOS
#
$(BUILD_DIR)\canl.lib: $(OBJFILES)
	if exist $@ del $@
    $(LIBEXE) $(LIBFLAGS) $@ @<<
$(OBJS), nul
<<


#
# Target canlib.dll is for Win16
# Will fail first time we build it, if thk16.obj isn't built & installed yet.
#
$(BUILD_DIR)\canlib.dll: $(OBJFILES) $(BUILD_DIR)\version.res $(BUILD_DIR)\ver31.res canlib.def
   $(LINK) $(LFLAGS) $(LFLAGS_DLL) @<<
$(OBJFILES)+$(GLOBAL_LIB_DIR)\thk16.obj
$@
$(BUILD_DIR)\canlib.map
$(LIBS_DLL)
canlib.def
<<
#   $(RC) $(RCFLAGS) -fo$(BUILD_DIR)\version.res $@
    copy $@ $(BUILD_DIR)\canlib31.dll
    $(THUNK_RC) $(RCFLAGS) -40 $(BUILD_DIR)\version.res $@
    $(RC) $(RCFLAGS) $(BUILD_DIR)\ver31.res $(BUILD_DIR)\canlib31.dll
    if exist $@ $(IMPLIB) $(IMPLIBFLAGS) $(@R).lib $@



setup install release:
    echo.


clean:
	-for %i in ($(FILES_STD_CLEAN)) do del %i


veryclean: clean
	-for %i in ($(FILES_EXTRA_CLEAN)) do del %i



$(BUILD_DIR)\canlib.obj: canlib.c
$(BUILD_DIR)\global.obj: global.c
$(BUILD_DIR)\irq.obj: irq.c
$(BUILD_DIR)\libcanc.obj: libcanc.c
$(BUILD_DIR)\libpccan.obj: libpccan.c
$(BUILD_DIR)\misc.obj: misc.c
$(BUILD_DIR)\timer.obj: timer.c
$(BUILD_DIR)\version.res: version.rc
$(BUILD_DIR)\ver31.res: ver31.rc
