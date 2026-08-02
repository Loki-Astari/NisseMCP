THORSANVIL_ROOT				?= $(realpath ../../)
TARGET						= NisseMCP.slib

FILE_WARNING_FLAGS			+= -Wno-missing-braces -Wno-dollar-in-identifier-extension -Wno-gnu-designator
LINK_LIBS					+= Nisse ThorsSocket ThorSerialize ThorsLogging
UNITTEST_LINK_LIBS			+= Nisse ThorsSocket ThorSerialize ThorsLogging
LDLIBS_EXTERN_BUILD			+= event  BoostCoRoutine Boost

include $(THORSANVIL_ROOT)/build/tools/Makefile
