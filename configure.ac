AC_INIT([NisseMCP], [0.1], [Loki.Astari+NisseMCP@Email.com])
AC_PREREQ([2.65])

AC_CONFIG_MACRO_DIR([build/autotools/m4])
AC_CONFIG_AUX_DIR([build/autotools/build])

AX_THOR_FUNC_INIT_BUILD([NisseMCP], [src/NisseMCP/NisseMCP.cpp])


#
# Add extra configuration here
#
AX_THOR_CHECK_USE_MAGIC_ENUM
AX_THOR_CHECK_USE_THORS_SERIALIZE_HEADER_ONLY
AX_THOR_CHECK_USE_THORS_SERIALIZE([ThorSerialize ThorsSocket ThorsLogging])


LT_INIT

AX_THOR_FEATURE_HEADER_ONLY_VARIANT([NISSEMCP])
AX_THOR_FUNC_TERM_BUILD([NISSEMCP], [src/NisseMCP/NisseMCPConfig.h:config.h.in])

AC_OUTPUT
