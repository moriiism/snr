#!/usr/bin/env python

"""
ofmovsp.py

2017-06-29 M.Morii
optical flow for radio blob, using sextractor and sparse optflow

"""

import sys
import os
import re

#
# parameter
#

ofmovsp_dir = '/home/morii/work/github/moriiism/snr/script/ofmovsp'
sextractor_cmd = '/soft/sextractor/2.19.5/bin/sex'
sextractor_config_dir = ofmovsp_dir + '/' + 'data'
sextractor_config_file = sextractor_config_dir + '/' + 'default.sex'

sextractor_config_filter_name = sextractor_config_dir + '/' + 'gauss_1.5_3x3.conv'
#sextractor_config_filter_name = sextractor_config_dir + '/' + 'gauss_5.0_9x9.conv'
# sextractor_config_filter_name = sextractor_config_dir + '/' + 'mexhat_1.5_5x5.conv'

sextractor_config_parameters_name = sextractor_config_dir + '/' + 'default.param'

#
# main
#

argvs = sys.argv
argc = len(argvs)
print argvs
print argc
if (argc != 5):
    print 'Usage: ofmovsp.py  in_list  subimg_dat  outdir  outfile_head'
    quit()

in_list      = argvs[1]
subimg_dat   = argvs[2]
outdir       = argvs[3]
outfile_head = argvs[4]

if(not os.path.exists(outdir)):
    os.mkdir(outdir)


#
# load in_list
#

file_arr = []
for line in open(in_list, 'r'):
    line = line.rstrip()
    items = line.split(' ')
    file_arr.append( items[0] )

#
# detect source
#

ifile = 0
for file in file_arr:
    print file
    sextractor_catalog_name = outdir + '/' + 'out_%3.3d.dat' % ifile
    cmd = sextractor_cmd + \
    ' -c ' + sextractor_config_file + \
    ' -FILTER_NAME ' + sextractor_config_filter_name + \
    ' -PARAMETERS_NAME ' + sextractor_config_parameters_name + \
    ' -CATALOG_NAME ' +  sextractor_catalog_name + \
    ' ' + file

    ## print cmd
    os.system(cmd)

    src_file = outdir + '/' + 'src_%3.3d.dat' % ifile
    fp_out = open(src_file, 'w')

    for line in open(sextractor_catalog_name, 'r'):
        line = line.rstrip()
        if(line[0] == '#'):
            continue
        items = line.split()
        str = '%s %s\n' % (items[1], items[2])
        fp_out.write(str)

    ifile = ifile + 1



#
# optical flow for two images
#
#

cmd_ofimg2sp = '/home/morii/work/github/moriiism/snr/util/ofimg2sp/ofimg2sp'

ifile = 0
for iarr in range(len(file_arr) - 1):
    src_file = outdir + '/' + 'src_%3.3d.dat' % iarr
    outfile_head = 'radio_%3.3d' % iarr

    cmd = cmd_ofimg2sp + ' ' \
        + file_arr[iarr] + ' ' \
        + file_arr[iarr + 1] + ' ' \
        + subimg_dat + ' ' \
        + src_file + ' ' \
        + outdir + ' ' \
        + outfile_head

    print cmd
    os.system(cmd)
