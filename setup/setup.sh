#
# setup.sh
#
# setup script for moriiism/snr
#
# 2017.02.03  M.Morii
#

#
# This script must be loaded by "source" command of bash,
# before using moriiism/snr
#

##
## cfitsio
##

# export LD_LIBRARY_PATH=/soft/cfitsio/3410/lib:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/soft/heasoft/heasoft-6.16/x86_64-unknown-linux-gnu-libc2.12/lib:${LD_LIBRARY_PATH}


##
## gsl
##

export LD_LIBRARY_PATH=/soft/gsl/2.3/lib:${LD_LIBRARY_PATH}

##
## root
##

export LD_LIBRARY_PATH=/soft/root/6.08.02/lib:${LD_LIBRARY_PATH}
export PATH=/soft/root/6.08.02/bin:${PATH}

alias root="root -l"

##
## OpenBLAS
##

export LD_LIBRARY_PATH=/soft/OpenBLAS/bcfc298/lib:${LD_LIBRARY_PATH}

##
## gotoblas2
##

#export LD_LIBRARY_PATH=/home/morii/admin/GotoBLAS2/GotoBLAS2:${LD_LIBRARY_PATH}
#export GOTO_NUM_THREADS=8

##
## openCV
## 

export LD_LIBRARY_PATH=/soft/opencv/3.2.0/lib:${LD_LIBRARY_PATH}

##
## change the terminal title
##

SNRTOOL_VER=master
termtitle="snrtool("$SNRTOOL_VER") "
PROMPT_COMMAND='echo -ne "\033]0;${termtitle}\007"'
