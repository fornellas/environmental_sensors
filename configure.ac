#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(
	[libopencm3-fpo-template],
	[0.0.0],
	[https://github.com/fornellas/libopencm3-fpo-template/issues],
	[libopencm3-fpo-template],
	[https://github.com/fornellas/libopencm3-fpo-template/])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AM_SILENT_RULES([yes])
AC_REQUIRE_AUX_FILE([libopencm3.mk])

##
## Canonical
##

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_MSG_CHECKING([for valid HOST])
if test "x$host" != xarm-none-eabi ; then
	AC_MSG_ERROR([Invalid host: this must be cross-compiled with --host=arm-none-eabi.])
fi
AC_MSG_RESULT([ok])

##
## libopencm3
##

AC_SUBST([OPENCM3_DIR], ["$(realpath "$(dirname "$0")")/libopencm3"])
# CFLAGS
AC_MSG_CHECKING([required libopencm3 stm32f4 CFLAGS])
CFLAGS_OPENCM3="$(make --silent -f ${srcdir}/build-aux/libopencm3.mk OPENCM3_DIR="$OPENCM3_DIR" DEVICE=stm32f4 ARCH_FLAGS)" || exit 1
AC_SUBST([CFLAGS_OPENCM3], [$CFLAGS_OPENCM3])
AC_MSG_RESULT([ok])
# LDFLAGS
LDFLAGS_OPENCM3="-L$OPENCM3_DIR/lib"
AC_SUBST([LDFLAGS_OPENCM3], [$LDFLAGS_OPENCM3])
# LIBS
LIBS_OPENCM3="-lopencm3_stm32f4 -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group"
AC_SUBST([LIBS_OPENCM3], [$LIBS_OPENCM3])
# CPPFLAGS
AC_MSG_CHECKING([required libopencm3_stm32f4 CPPFLAGS])
CPPFLAGS_OPENCM3="$(make --silent -f ${srcdir}/build-aux/libopencm3.mk OPENCM3_DIR="$OPENCM3_DIR" DEVICE=stm32f4 CPPFLAGS)" || exit 1
AC_SUBST([CPPFLAGS_OPENCM3], [$CPPFLAGS_OPENCM3])
AC_MSG_RESULT([ok])

##
## eglib
##

AX_SUBDIRS_CONFIGURE([eglib], [], [], [--with-libopencm3="$OPENCM3_DIR"], [])

##
## Misc
##

# Avoid "undefined reference to `_exit'" during tests
LIBS_NOSYS_SPECS="--specs=nosys.specs"

# Common warning flags
CFLAGS_WARNINGS="-Wextra -Wshadow -Wno-unused-variable -Wimplicit-function-declaration"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Wall -Wundef"
CFLAGS_WARNINGS="$CFLAGS_WARNINGS -Werror -Wfatal-errors"
AC_SUBST([CFLAGS_WARNINGS], [$CFLAGS_WARNINGS])

##
## Checks for programs.
##

# CC
: ${CFLAGS="-g -O3"}
ORIGINAL_LIBS="$LIBS"
LIBS="$LIBS_NOSYS_SPECS $LIBS"
AC_PROG_CC
LIBS="$ORIGINAL_LIBS"

# OBJCOPY
AC_CHECK_TOOL([OBJCOPY], [objcopy])
if test "x$OBJCOPY" = x ; then
	AC_MSG_FAILURE([objcopy not found!])
fi

# GDB
AC_CHECK_PROG([GDB], [gdb-multiarch], [gdb-multiarch], [no])
if test "x$GDB" = xno ; then
	AC_CHECK_TOOL([GDB], [gdb], [no])
	if test "x$GDB" = xno ; then
		AC_CHECK_PROG([GDB], [gdb], [gdb], [no])
		if test "x$GDB" != xno ; then
			AC_MSG_WARN([Using plain gdb: beware it may not support $host!])
		fi
	fi
fi
AM_CONDITIONAL([HAS_GDB], [test "x$GDB" != xno])
AC_SUBST([GDB], [$GDB])

##
## Checks for libraries.
##

##
## Checks for header files.
##

ORIGINAL_CPPFLAGS="$CPPFLAGS" ; CPPFLAGS="$CPPFLAGS_OPENCM3 $CPPFLAGS"
AC_CHECK_HEADERS([libopencm3/stm32/rcc.h], [], AC_MSG_FAILURE([libopencm3/stm32/rcc.h not found!]))
CPPFLAGS="$ORIGINAL_CPPFLAGS"

##
## Checks for typedefs, structures, and compiler characteristics.
##

##
## Checks for library functions.
##

# N/A

AC_CONFIG_FILES([Makefile])
AC_OUTPUT