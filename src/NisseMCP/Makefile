THORSANVIL_ROOT				?= $(realpath ../../)
TARGET						= NisseMCP.slib

FILE_WARNING_FLAGS			+= -Wno-missing-braces -Wno-dollar-in-identifier-extension -Wno-gnu-designator
LINK_LIBS					+= Nisse ThorSerialize ThorsLogging
UNITTEST_LINK_LIBS			+= Nisse ThorSerialize ThorsLogging


include $(THORSANVIL_ROOT)/build/tools/Makefile
