# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([pdbfs], [0.0.3], [mlpalii@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror gnu])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C_O

# Checks for required tools.
AC_CHECK_PROG([RSYNC], [rsync], [$as_dir/$ac_word$ac_exec_ext], [not found])
if test "${RSYNC}" = "not found" ; then
  AC_MSG_ERROR([rsync executable not found in PATH, get it from <http://rsync.samba.org/>])
else
  AC_DEFINE_UNQUOTED([RSYNC], ["${RSYNC}"], [Define the location of the rsync executable.])
fi

# Checks for libraries.
AC_CHECK_LIB([fuse], [fuse_main])
AC_CHECK_LIB([pthread], [pthread_mutex_init])
AC_CHECK_LIB([z], [gzopen])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/statvfs.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNCS([memset mkdir])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
