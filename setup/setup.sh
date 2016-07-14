#
# setup.sh
#
# setup script for moriiism/snr
#
# 2016.07.08  M.Morii
#

#
# This script must be loaded by "source" command of bash,
# before using moriiism/snr
#

##
## cfitsio
##

export LD_LIBRARY_PATH=/soft/cfitsio/3380/lib:${LD_LIBRARY_PATH}

##
## gotoblas2
##

export LD_LIBRARY_PATH=/home/morii/admin/GotoBLAS2/GotoBLAS2:${LD_LIBRARY_PATH}
export GOTO_NUM_THREADS=8

##
## openblas
##

#export LD_LIBRARY_PATH=/soft/OpenBLAS/12ab180/lib:${LD_LIBRARY_PATH}
#export OPENBLAS_NUM_THREADS=8

##
## openmpi
##

#export LD_LIBRARY_PATH=/usr/lib64/openmpi/lib:${LD_LIBRARY_PATH}
#export PATH=$PATH:/usr/lib64/openmpi/bin

##
## change the terminal title
##

SNRTOOL_VER=master
termtitle="snrtool("$SNRTOOL_VER") "
PROMPT_COMMAND='echo -ne "\033]0;${termtitle}\007"'
