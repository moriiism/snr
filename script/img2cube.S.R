#!/usr/local/bin/Rscript

###
### img2cube.S.R
###
### 2016.07.11 M.Morii
###

require(FITSio)
library("svd")

snrtooldir = "/home/morii/work/github/moriiism/snr"

args <- commandArgs(TRUE)
in.file  = args[1]
subcube  = args[2]
outdir   = args[9]
outfile.head = args[10]

dir.create(outdir, recursive = TRUE)

fits <- readFITS(in.file)
subimg.df <- read.table(subcube, comment.char = "#")

pos.x.first = subimg.df[1,1]
pos.y.first = subimg.df[1,2]
pos.t.first = subimg.df[1,3]
pos.x.last  = subimg.df[2,1]
pos.y.last  = subimg.df[2,2]
pos.t.last  = subimg.df[2,3]
npos.x = pos.x.last - pos.x.first + 1
npos.y = pos.y.last - pos.y.first + 1
npos.t = pos.t.last - pos.t.first + 1
nrow = npos.x * npos.y
ncol = npos.t

### time st
time.st = Sys.time()

mat =  matrix(as.vector(
    fits$imDat[pos.x.first:pos.x.last, pos.y.first:pos.y.last, pos.t.first:pos.t.last]),
    nrow=nrow, ncol=ncol)
rm(fits)



cube.org = array(mat, dim=c(npos.x, npos.y, npos.t))
cube.sp  = array(godec.ret[[1]], dim=c(npos.x, npos.y, npos.t))
cube.lr  = array(godec.ret[[2]], dim=c(npos.x, npos.y, npos.t))
cube.err = array(godec.ret[[3]], dim=c(npos.x, npos.y, npos.t))

cube.img = array(godec.ret[[4]], dim=c(npos.x, npos.y, nrank))
image.time = array(godec.ret[[5]], dim=c(nrank, ntime))

file.org = sprintf("%s/%s_org.fits", outdir, outfile.head)
file.sp  = sprintf("%s/%s_sp.fits", outdir, outfile.head)
file.lr  = sprintf("%s/%s_lr.fits", outdir, outfile.head)
file.err = sprintf("%s/%s_err.fits", outdir, outfile.head)
file.img = sprintf("%s/%s_img.fits", outdir, outfile.head)
file.time = sprintf("%s/%s_time.fits", outdir, outfile.head)
file.rec = sprintf("%s/%s_rec.fits", outdir, outfile.head)

writeFITSim(cube.org, file=file.org)
writeFITSim(cube.sp, file=file.sp)
writeFITSim(cube.lr, file=file.lr)
writeFITSim(cube.err, file=file.err)
writeFITSim(cube.img, file=file.img)
writeFITSim(image.time, file=file.time)
writeFITSim(cube.rec, file=file.rec)

### time ed
time.ed = Sys.time()
as.difftime(time.ed - time.st)
