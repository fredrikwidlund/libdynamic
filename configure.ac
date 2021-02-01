AC_INIT([libdynamic],[2.3.0],[fredrik.widlund@gmail.com])
AC_CONFIG_AUX_DIR(autotools)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign no-define])

: ${CFLAGS="-Wall -Wextra -Wpedantic -g -O3 -march=native -flto"}

AM_PROG_AR
LT_INIT
AM_PROG_CC_C_O

AC_PREFIX_DEFAULT(/usr)
AC_CONFIG_FILES([Makefile example/Makefile libdynamic.pc])
AC_OUTPUT
