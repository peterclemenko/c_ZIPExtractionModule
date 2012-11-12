dnl
dnl AM_PATH_POCO(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
oco
AC_DEFUN([AM_PATH_POCO],
[

AC_ARG_WITH(poco-prefix,[  --with-poco-prefix=PFX   Prefix where Poco is installed (optional)],
            poco_config_prefix="$withval", poco_config_prefix="")
AC_ARG_WITH(poco-exec-prefix,[  --with-poco-exec-prefix=PFX  Exec prefix where poco is installed (optional)],
            poco_config_exec_prefix="$withval", poco_config_exec_prefix="")

  if test x$poco_config_exec_prefix != x ; then
     poco_config_args="$poco_config_args --exec-prefix=$poco_config_exec_prefix"
     if test x${POCO_CONFIG+set} != xset ; then
        POCO_CONFIG=$POCO_CONFIG_exec_prefix/bin/poco-config
     fi
  fi
  if test x$POCO_CONFIG_prefix != x ; then
     POCO_CONFIG_args="$POCO_CONFIG_args --prefix=$POCO_CONFIG_prefix"
     if test x${poco_CONFIG+set} != xset ; then
        POCO_CONFIG=$POCO_CONFIG_prefix/bin/poco-config
     fi
  fi

  AC_PATH_PROG(POCO_CONFIG, poco-config, no)
  poco_version_min=$1

  AC_MSG_CHECKING(for Poco - version >= $poco_version_min)
  no_poco=""
  if test "$POCO_CONFIG" = "no" ; then
    AC_MSG_RESULT(no)
    no_poco=yes
  else
    POCO_CFLAGS=`$POCO_CONFIG --cflags`
    POCO_LIBS=`$POCO_CONFIG --libs`
    poco_version=`$POCO_CONFIG --version`

    poco_major_version=`echo $poco_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    poco_minor_version=`echo $poco_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    poco_micro_version=`echo $poco_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    poco_major_min=`echo $poco_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    if test "x${poco_major_min}" = "x" ; then
       poco_major_min=0
    fi
    
    poco_minor_min=`echo $poco_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    if test "x${poco_minor_min}" = "x" ; then
       poco_minor_min=0
    fi
    
    poco_micro_min=`echo $poco_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x${poco_micro_min}" = "x" ; then
       poco_micro_min=0
    fi

    poco_version_proper=`expr \
        $poco_major_version \> $poco_major_min \| \
        $poco_major_version \= $poco_major_min \& \
        $poco_minor_version \> $poco_minor_min \| \
        $poco_major_version \= $poco_major_min \& \
        $poco_minor_version \= $poco_minor_min \& \
        $poco_micro_version \>= $poco_micro_min `

    if test "$poco_version_proper" = "1" ; then
      AC_MSG_RESULT([$poco_major_version.$poco_minor_version.$poco_micro_version])
    else
      AC_MSG_RESULT(no)
      no_poco=yes
    fi
  fi

  if test "x$no_poco" = x ; then
     ifelse([$2], , :, [$2])     
  else
     POCO_CFLAGS=""
     POCO_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(POCO_CFLAGS)
  AC_SUBST(POCO_LIBS)
])



