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

export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}


##
## cfitsio
##

export LD_LIBRARY_PATH=/soft/cfitsio/3380/lib:${LD_LIBRARY_PATH}


##
## change the terminal title
##

SNRTOOL_VER=master
termtitle="snrtool("$SNRTOOL_VER") "
PROMPT_COMMAND='echo -ne "\033]0;${termtitle}\007"'
