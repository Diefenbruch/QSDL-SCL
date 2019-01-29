#
# Makefile fuer die SCL (Simulation Class Library)
#
#
# Author: Marc Diefenbruch
# $Date: 1998/02/19 16:16:22 $
#
# (C) 1996 Universitaet GH Essen
#
# Dieses Makefile stellt folgende Operationen zur Verfuegung:
#
# -) make [all]     : Library und Testprogramm erzeugen
# -) make lib       : Library erzeugen
# -) make release   : Neue Release der DS fuer Benutzer zugaenglich machen
# -) make install   :  "      "     "   "   "     "         "         "
# -) make install-lib: Neue Version der DS-Bibiothek zugaenglich machen
# -) make install-includes: Neue Version der DS-Header_files zugaenglich machen
# -) make checkout  : alle Projektdateien auschecken
# -) make checkout-lock: alle Projektdateien zum Aendern auschecken
# -) make depend    : alle Abhaengigkeiten zwischen Projektdateien feststellen
# -) make clean     : Objektdateien (*.o) und temporaere Dateien entfernen
# -) make veryclean : make clean + rcsclean + alle Postscriptdateien entfernen
# -) make git-pull  : pull sources from github
# -) make git-push  : push sources to github
# -) make git-add   : add changed sources to git staging
# -) make git-commit: commit staged sources to git HEAD
# -) make backup    : Backup von allen *.cpp *.h Dateien und dem Makefile
# -) make find-error: Compilierungsfehler anzeigen
#
# Der Aufbau dieses Makefiles gliedert sich wie folgt:
#
#  1. Makefiledirektiven
#  2. Pfade und Dateinamen
#  3. Programme, Kommandos, Kommandooptionen
#  4. Compilierungs- und Linkeroptionen
#  5. Quelldateien des Projekts
#  6. Objektdateien des Projekts
#  7. Makefileregeln (Operationen, s.o.)
#

###############################
# figure out host system type #
###############################

## Don't rely on $HOSTTYPE being set correctly!
## Try to figure it out ourselves:

## Prevent make from interpreting special characters:
SCRIPT := \
	case `uname -s` in\
    	Linux)	case `uname -m` in\
					i?86) echo ix86-linux;;\
					arm*) echo arm-linux;;\
					sparc) echo sparc-linux;;\
					*) echo unknown-linux;;\
				esac;;\
		SunOS)	echo `uname -p`-SunOS;;\
		*) echo unknown-unknown;;\
	esac

## Run script and store result in variable:
QUEST_HOSTTYPE := $(shell $(SCRIPT))


##########################
# 0. Globale Umschalter: #
##########################

ifeq ($(QUEST_HOSTTYPE), sparc-SunOS)

#TOOLS = SUN
TOOLS = GNU

else

TOOLS = GNU

endif

#PROFILING = yes
PROFILING = no

##########################
# 1. Makefiledirektiven: #
##########################

.SILENT:
                         # alle Make Operationen ohne Ausgaben

.PHONY: Makefile default clean all release .depend \
        install install-lib install-includes release \
        postscript print
                         # Welche Operationen sollen gespraechig sein?

.SUFFIXES: .cpp .h .o
                         # verwendete Dateiendungen

############################
# 2. Pfade und Dateinamen: #
############################

INCDIR = ../../include
                         # Verzeichnis der SCL-Includedateien fuer die
                         # Benutzer der SCL
LIBDIR = ../../lib/$(QUEST_HOSTTYPE)
                         # Verzeichnis der SCL-Library fuer die
                         # Benutzer der SCL
BINDIR = ../../bin/$(QUEST_HOSTTYPE)
                         # Verzeichnis in dem die Binaerdateien gespeichert
                         # werden sollen
OBJBASEDIR = obj
                         # Verzeichnis unter dem die Objektdateien aller Hosts
                         # gespeichert werden sollen
OBJDIR = $(OBJBASEDIR)/$(QUEST_HOSTTYPE)
                         # Verzeichnis in dem die Objektdateien gespeichert
                         # werden sollen
PSDIR = ps
                         # Directory in dem die Postscriptversionen der
                         # Dateien gespeichert werden sollen
LOGFILE = $(USER).$(QUEST_HOSTTYPE).make.log
                         # Protokollierungsdatei fuer den make-Vorgang
DEPFILE = .depend.$(QUEST_HOSTTYPE)
                         # Dependency-Datei (automatisch generiert)

OUTPUT  = $(OBJDIR)/libSCL.a

BACKUP = scl
                         # Name des Backupfiles (ohne Endungen!)

TEMPLATE_INST_DIR = Templates.DB

##############################################
# 3. Programme, Kommandos, Kommandooptionen: #
##############################################

SHELL = /bin/sh
                         # Shell
LEX = flex
                         # Kommando zum Starten des lexikalischen Analysators
LEXFLAGS = -i
                         # Flags fuer Lexer-Generator
YACC = bison
                         # Kommando zum Starten des Parser-Generators
YACCFLAGS = -d -v
                         # Flags fuer Parser-Generator
TAR = tar cf
                         # Befehl zum Erzeugen eines Sourcecode-Archievs
UNTAR = tar xf
                         # Befehl zum Expandieren eines Sourcecode-Archievs
TAR_SUFFIX = tar
                         # Endung welche das tar-Kommando verwendet
COMPRESS = gzip -9
                         # Befehl zum Komprimieren von Dateien
UNCOMPRESS = gzip -d
                         # Befehl zum Dekomprimieren von Dateien

COMPRESS_SUFFIX = gz
                         # Endung welche das Komprimierungsprogramm verwendet
EDITOR = jove
                         # Name des verwendeten Editors
TAGS = ctags -t
                         # Programm zum Erzeugen von Sourcebrowsing-Infos
                         # (wo ist eine bestimmte Funktion definiert?, etc)
MAKE_PS = a2ps
                         # Kommando zum Erzeugen von Postscript aus ASCII
MAKE_PS_FLAGS = -o
                         # Option fuer MAKE_PS
PS_SUFFIX = ps

PRINT = lp -c -d $(PRINTER)
                         # Druckkommando

ifeq ($(TOOLS), GNU)     # GNU-Version ?

CC = gcc
                         # Name des C-Compilers
C++ = g++
                         # Name des C++-Compilers

AR = ar
                         # Name des Archivers (zum Erzeugen von Libraries)
ARFLAGS = r
                         # Flags fuer den Archiver
else                     # SUN-Version

CC = cc
                         # Name des C-Compilers
C++ = CC
                         # Name des C++-Compilers
AR = $(C++)
                         # Name des Archivers (zum Erzeugen von Libraries)
ARFLAGS = -xar -o
                         # Flags fuer den Archiver
endif

RCSCLEAN = rcsclean
                         # Befehl zum "Aufraeumen" des Projekts (Loeschen
                         # der nicht mit Lock Option "ausgecheckten"
                         # Dateien
RM = /bin/rm -f
                         # Befehl zum Loeschen von Dateien
MV = /bin/mv
                         # Befehl zum Verschieben von Dateien
CP = /bin/cp -p
                         # Befehl zum Verschieben von Dateien
CHMOD = /bin/chmod
                         # Befehl zum Aendern der Dateiattribute
TOUCH = touch
                         # Befehl zum Aktualisieren des Dateidatums
MKDIR = mkdir -p
                         # Befehl zum Erzeugen von Directories
ifeq ($(TOOLS), GNU)     # GNU-Version ?

MAKE_DEPEND = $(C++) -M
                         # Befehl zum Erzeugen der Abhaengigkeiten

else

MAKE_DEPEND = $(C++) -xM
                         # Befehl zum Erzeugen der Abhaengigkeiten

endif

STRIP = strip
                         # Befehl zum entfernen von Symbolen aus Binaerfiles
CI = ci

SED = sed
                         # Name des Stream-Editors sed
COFLAGS += -M -q
                         # Tell co to set the file date to the date of the
                         # last modification of the file.

#########################################
# 4. Compilierungs- und Linkeroptionen: #
#########################################

INCLUDES = -I$(QUEST_ADDITIONAL_INC_DIR) -I. -I$(INCDIR) 
                         # Include-Verzeichnisse fuer die Compiler
                         # QUEST_ADDITIONAL_INC_DIRS may be used to specify
                         # system specific include pathes.

CTHREAD_TYPE = $(QUEST_THREAD_TYPE)
                         # Verwendete Threads (Solaris, PThreads, etc.)

ifeq ($(TOOLS), GNU)     # GNU-Version ?

CDEBUG = -O6 -pipe
#CDEBUG += -DDMALLOC -DDMALLOC_FUNC_CHECK
                         # Debug-/Optimierungsoptionen

CFLAGS = -Wall 
#-pedantic
                         # Compilierungsoptionen
TFLAGS = -fno-implicit-templates
                         # Template-Flags

LFLAGS = $(CDEBUG)
                         # Linkeroptionen

CFLAGS += -D_SC_BUILD

ifeq ($(PROFILING), yes) # Profiling einschalten ?

CFLAGS += -pg
LFLAGS += -pg

endif

else                     # Sun-Version !

CDEBUG = -O
#CDEBUG += -DDMALLOC -DDMALLOC_FUNC_CHECK
                         # Debug-/Optimierungsoptionen
CFLAGS = -mt -DUSERNAME=$(USER)
                         # Compilierungsoptionen
TFLAGS = 
                         # Template-Flags

LFLAGS = $(CDEBUG) -mt
                         # Linkeroptionen

CFLAGS += -D_SC_BUILD

ifeq ($(PROFILING), yes) # Profiling einschalten ?

CFLAGS += -xpg
LFLAGS += -xpg

endif

endif

CFLAGS += $(CDEBUG)
#CFLAGS += $(CTHREAD_TYPE)
CFLAGS += $(SCL_EXTRA_FLAGS)

#################################
# 5. Quelldateien des Projekts: #
#################################

SRCS = \
	SCObject.cpp\
	\
	SCMem.cpp\
	SCStack.cpp\
	SCStackElem.cpp\
	SCTable.cpp\
	SCStateSpace.cpp\
	SCHashTable.cpp\
	SCBitState.cpp\
	SCFullState.cpp\
	SCHeap.cpp\
	\
	SCList.cpp \
	SCListSave.cpp \
	SCDataType.cpp \
	SCArray.cpp \
	SCString.cpp \
	SCByte.cpp \
	SCCharstring.cpp \
	SCPowerset.cpp \
	SCSyntype.cpp \
	\
	SCType.cpp \
	SCProcessType.cpp \
	SCProcedureType.cpp \
	SCRequestType.cpp \
	SCSignalType.cpp \
	SCTimerType.cpp \
	SCStateType.cpp \
	\
	SCSystem.cpp \
	SCSignal.cpp \
	SCDelayedSignal.cpp \
	SCProcess.cpp \
	SCProcessList.cpp \
	SCProcedure.cpp \
	SCProcedureList.cpp \
	SCAutomaton.cpp \
	SCRunnable.cpp \
	SCTimerControl.cpp \
	SCTimer.cpp \
	SCTimeEvent.cpp \
	SCTimeEventList.cpp \
	SCPath.cpp \
	SCTransition.cpp \
	\
	SCMachine.cpp \
	SCMachineList.cpp \
	SCMachineLtd.cpp \
	SCMachineInf.cpp \
	SCMachineFCFS.cpp \
	SCMachineFCFSPNP.cpp \
	SCMachineFCFSPP.cpp \
	SCMachineIS.cpp \
	SCMachineLCFSPP.cpp \
	SCMachinePS.cpp \
	SCMachineRandom.cpp \
	SCRequest.cpp \
	\
	SCThread.cpp \
	SCMacMutex.cpp\
	SCMacMainThread.cpp \
	SCMacSystemRunnable.cpp \
	SCMacSystemScheduler.cpp \
	SCMacSystemThread.cpp \
	SCPosixMutex.cpp\
	SCPosixMainThread.cpp \
	SCPosixSystemRunnable.cpp \
	SCPosixSystemScheduler.cpp \
	SCPosixSystemThread.cpp \
	\
	SCIndet.cpp \
	SCIndetSim.cpp \
	SCIndetStackTrace.cpp \
	SCIndetVal.cpp \
	SCActiveRunnable.cpp\
	SCEnabledTransition.cpp\
	SCScheduler.cpp \
	SCSocket.cpp \
	\
	SCRandBernoulli.cpp \
	SCRandConst.cpp \
	SCRandDouble.cpp \
	SCRandEmpirical.cpp \
	SCRandErlang.cpp \
	SCRandLong.cpp \
	SCRandNegExp.cpp \
	SCRandNormal.cpp \
	SCRandom.cpp \
	SCRandPoisson.cpp \
	SCRandUniform.cpp \
	SCRandUniLong.cpp \
	\
	SCSensor.cpp \
	SCSensorCounter.cpp \
	SCSensorFrequency.cpp \
	SCSensorDynFreq.cpp \
	SCSensorStatFreq.cpp \
	SCSensorTally.cpp \
	SCSensorQueueLength.cpp \
	\
	SCTraceControl.cpp \
	SCTrace.cpp \
	SCTraceEventMSC.cpp \
	SCTraceSensor.cpp \
	SCTraceSocket.cpp \
	\
	SCDebug.cpp

INSTANT = SCTemplates.cpp

INSTALL_INLINES = \
      SCArray.inl.h\
      SCString.inl.h\
      SCSyntype.inl.h

INLINES = $(INSTALL_INLINES)\
      SCByte.inl.h\
      SCCharstring.inl.h\
      SCHashTable.inl.h\
      SCDelayedSignal.inl.h\
      SCEnabledTransition.inl.h\
      SCAutomaton.inl.h\
      SCIndetSim.inl.h\
      SCIndetStackTrace.inl.h\
      SCIndetVal.inl.h\
      SCMachine.inl.h\
      SCSignal.inl.h\
      SCPowerset.inl.h\
      SCProcedure.inl.h\
      SCProcedureType.inl.h\
      SCProcess.inl.h\
      SCProcessType.inl.h\
      SCRandBernoulli.inl.h\
      SCRandConst.inl.h\
      SCRandNegExp.inl.h\
      SCRandNormal.inl.h\
      SCRandPoisson.inl.h\
      SCRandUniLong.inl.h\
      SCRandUniform.inl.h\
      SCRequest.inl.h\
      SCRunnable.inl.h\
      SCScheduler.inl.h\
      SCStateType.inl.h\
      SCTimer.inl.h\
      SCTrace.inl.h\
      SCTransition.inl.h\
      SCType.inl.h

TEMPLATES = \
      SCArray.tmpl.h\
      SCString.tmpl.h\
      SCHeap.tmpl.h\
      SCList.tmpl.h\
      SCListSave.tmpl.h\
      SCSyntype.tmpl.h\
      SCTable.tmpl.h

HDRS = \
    SCBasicTypes.h \
    SCEnvironment.h \
    SCListCons.h \
    SCListIter.h \
    SCSignalList.h \
    SCStackElemList.h \
    SCStackElemListTable.h\
    SCStream.h\
    SCDelayedSignalList.h\
    SCActiveRunnableList.h\
    SCRand48Seed.h\
    SCRandMGenSeed.h\
    SCRandLGenSeed.h\
    SCRequestList.h\
    SCEnabledTransitionList.h\
    SCThreadTypes.h\
    SCMutexTypes.h\
    SCMainThreadTypes.h\
    SCTimerList.h\
    SCTimeEventTable.h\
    SCTimeEventHeap.h\
    SCTraceList.h\
    SCTraceTypes.h\
    SCTransitionList.h\
    SCStateTypeList.h\
    SCProcessTypeList.h\
    SCAutomatonTable.h\
    SCProcedureTypeList.h\
    SCSignalTable.h \
    SCTransitionListTable.h \
    SCTimerTable.h \
    SCTimerListTable.h \
    SCSignalTypeTable.h \
    SCRequestTypeTable.h \
    SCStateTypeTable.h \
    SCProcessTypeTable.h \
    SCProcedureTypeTable.h \
    SCRunnableTypes.h \
    SCSchedulerTypes.h \
    SCSensorSignal.h \
    SCSensorRequest.h \
    SCSensorProcess.h \
    SCSensorMachine.h \
    SCSensorList.h \
    SCSensorTable.h \
    SCMemTable.h

##################################
# 6. Objektdateien des Projekts: #
##################################

HEADERS	= $(SRCS:.cpp=.h) $(HDRS) $(TEMPLATES)
SOURCES = $(SRCS) $(INSTANT)
OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))
PSFILES	= $(HEADERS:.h=.ps)

######################
# 7. Makefileregeln: #
######################

all: clean-rubbish $(OBJDIR) $(OUTPUT)

$(OUTPUT): $(OBJS)
	echo Constructing $(OUTPUT) ...
	$(AR) $(ARFLAGS) $(OUTPUT) $(OBJS) \
		2>> $(LOGFILE)

$(OBJS): $(OBJDIR)

$(OBJDIR)/%.o : %.cpp
	echo Compiling $< ...
	$(C++) -c $(CFLAGS) $(TFLAGS) $(INCLUDES) -o $@  $< 2>>$(LOGFILE)

$(OBJBASEDIR):
	if [ ! \( -d $(OBJBASEDIR) \) ]; then \
		echo Creating $(OBJBASEDIR) ...; \
		$(MKDIR) $(OBJBASEDIR); fi

$(OBJDIR): $(OBJBASEDIR)
	if [ ! \( -d $(OBJDIR) \) ]; then \
		echo Creating $(OBJDIR) ...; \
		$(MKDIR) $(OBJDIR); fi

$(LIBDIR):
	if [ ! \( -d $(LIBDIR) \) ]; then \
		echo Creating $(LIBDIR) ...; \
		$(MKDIR) $(LIBDIR); fi

$(BINDIR):
	if [ ! \( -d $(BINDIR) \) ]; then \
		echo Creating $(BINDIR) ...; \
		$(MKDIR) $(BINDIR); fi

$(PSDIR):
	if [ ! \( -d $(PSDIR) \) ]; then \
		echo Creating $(PSDIR) ...; \
		$(MKDIR) $(PSDIR); fi

%.ps : %.h
	echo Generating   $(PSDIR)/$(notdir $@) form $< ...
	$(MAKE_PS) $(MAKE_PS_FLAGS) $(PSDIR)/$(notdir $@) $<

$(INCDIR): 
	@if [ ! \( -d $(INCDIR) \) ]; then \
		echo Creating $(INCDIR) ...; \
		$(MKDIR) $(INCDIR); fi

$(INCDIR)/SCL: $(INCDIR)
	@if [ ! \( -d $(INCDIR)/SCL \) ]; then \
		echo Creating $(INCDIR)/SCL ...; \
		$(MKDIR) $(INCDIR)/SCL; fi

$(DEPFILE):
	$(TOUCH) $(DEPFILE)

install-lib: $(OUTPUT) $(LIBDIR)
	echo Deleting old library from $(LIBDIR) ...
	-$(RM) $(LIBDIR)/$(OUTPUT)
	echo Installing new library in $(LIBDIR) ...
	$(CP)  $(OUTPUT) $(LIBDIR)

install-includes: $(HEADERS) $(INSTALL_INLINES) $(INCDIR)/SCL
	echo Deleting old include files from $(INCDIR)/SCL ...
	-$(RM) $(INCDIR)/SCL/*.h
	echo Installing new include files in $(INCDIR)/SCL ...
	for X in $(HEADERS) $(INSTALL_INLINES); do\
		$(CP) $${X} $(INCDIR)/SCL; done

install: install-includes install-lib

release: install

postscript: $(PSDIR) $(PSFILES)

print: postscript
	for X in $(PSFILES); do \
		echo Printing $(PSDIR)/$$X ...; \
		$(PRINT) $(PSDIR)/$$X ; done

backup: $(SOURCES) $(HEADERS) $(INLINES)
	-$(RM) $(BACKUP).$(TAR_SUFFIX) $(BACKUP).$(TAR_SUFFIX).$(COMPRESS_SUFFIX)
	$(TAR) $(BACKUP).$(TAR_SUFFIX)\
		$(SOURCES) $(HEADERS) $(INLINES) Makefile
	$(COMPRESS) $(BACKUP).$(TAR_SUFFIX)
	-$(MV) $(BACKUP).$(TAR_SUFFIX).$(COMPRESS_SUFFIX) $(BACKUP).taz

veryclean: clean clean-rcs 
	-$(RM) $(PSDIR)/*.$(PS_SUFFIX) *.$(PS_SUFFIX) *.$(TAR_SUFFIX) *.$(COMPRESS_SUFFIX) *.taz *tags 2!

clean:
	-$(RM) $(OBJS)
	-$(RM) -rf $(TEMPLATE_INST_DIR)
	-$(RM) $(LOGFILE)
	-$(RM) $(OUTPUT)
	-$(RM) *.bck

clean-rcs:
	-$(RCSCLEAN) 2> /dev/null

clean-rubbish:
	-$(RM) core* *.bak *~ $(LOGFILE)

find-error fe:
	$(EDITOR) -p $(LOGFILE)

checkout co:
	-$(CO) $(COFLAGS) $(HEADERS) $(INLINES)\
             $(SOURCES)

checkout-lock col:
	-$(CO) -l $(HEADERS) $(INLINES)\
             $(SOURCES)

checkin ci: clean-rcs
	-$(CI) -m. *.cpp *.h

replace-headers: $(HEADERS) $(INLINES)
	for X in $(HEADERS) $(INLINES); do \
		$(MV) $$X $$X.old; \
		$(SED) 's/Erstellt/Created /g' $$X.old >> $$X ; done

replace-sources: 
	for X in $(SOURCES); do \
		$(MV) $$X $$X.old; \
		$(SED) 's/Erstellt/Created /g' $$X.old >> $$X ; done

depend: $(HEADERS) $(INLINES) $(SOURCES)
	@echo Building dependency file $(DEPFILE) ...
	$(MAKE_DEPEND) $(SOURCES) $(INCLUDES) $(CFLAGS) > $(DEPFILE)
	$(SED) 's/^\(.*\)\.o/\$$(OBJDIR)\/\1\.o/g' $(DEPFILE) > $(DEPFILE).sed
	$(MV) $(DEPFILE).sed $(DEPFILE)


-include $(DEPFILE)

