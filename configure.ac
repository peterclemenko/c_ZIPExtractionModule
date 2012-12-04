#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])

AC_INIT(c_ZIPExtractionModule, 1.0.0)
AC_CONFIG_SRCDIR([ZipExtractionModule.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([foreign])
AM_PROG_LIBTOOL
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_LANG([C++])
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

# Not all compilers include /usr/local in the include and link path
if test -e /usr/local/include; then
    CFLAGS="$CFLAGS -I/usr/local/include"
    LDFLAGS="$LDFLAGS -L/usr/local/lib"
fi

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strtoul strtoull])

# Check if the module is under TskModules
if test -f ../TskModules/Makefile.am; then
   AC_SUBST(FW_LIBS, "../../lib/libtskframework.la ../../../tsk3/libtsk3.la")
   AC_SUBST(TSK_INCLUDE, "-I../../..")
   AC_SUBST(FW_INCLUDE,  "-I../..")
else
   # Not part of TskModules, check if Tsk and Framework has been installed
   AC_SUBST(FW_LIBS, "")
   AC_CHECK_HEADERS([tsk3/libtsk.h],[HAVE_TSK_INCLUDE=1],AC_MSG_ERROR([Tsk include files not found.]))
   AC_CHECK_LIB([tsk3],[main],[HAVE_TSK_LIB=1],AC_MSG_ERROR([Tsk library not found.]))
   if test "$HAVE_TSK_INCLUDE"; then
       AC_SUBST(TSK_INCLUDE, "")
       AC_SUBST(FW_INCLUDE,  "")
   else
       AC_SUBST(TSK_INCLUDE, "-I../../..")
       AC_SUBST(FW_INCLUDE,  "-I../..")
   fi
   if test "HAVE_TSK_LIB"; then
       AC_SUBST(FWLIBS, "")
   else
       AC_SUBST(FW_LIBS, "../../lib/libtskframework.la ../../../tsk3/libtsk3.la")
   fi
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
