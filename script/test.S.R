#!/usr/local/bin/Rscript

###
### nmf.S.R
###
### 2016.07.13 M.Morii
###

require(FITSio)
library("svd")
library("NMF")

snrtooldir = "/home/morii/work/github/moriiism/snr"
source( paste(snrtooldir, "scriptR/rlib/iolib.R", sep="/") )
source( paste(snrtooldir, "scriptR/rlib/binning.R", sep="/") )
source( paste(snrtooldir, "scriptR/rlib/vec_ope.R", sep="/") )

args <- commandArgs(TRUE)
in.file  = args[1]
subcube  = args[2]
nrank    = as.numeric(args[3])
outdir   = args[4]
outfile.head = args[5]

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

mat =  matrix(
    as.vector(
        fits$imDat[pos.x.first:pos.x.last,
                   pos.y.first:pos.y.last,
                   pos.t.first:pos.t.last]),
    nrow=nrow, ncol=ncol)
rm(fits)


ret.svd = svd(mat)
sv.ratio = ret.svd$d / ret.svd$d[1]
head(sv.ratio, n = 10)

### clean

mat.cl = matrix(0.0, nrow=nrow, ncol = ncol)
for(irank in 1:nrank){
    mat.cl = mat.cl +
        ret.svd$u[,irank] %*% t(ret.svd$v[,irank]) * ret.svd$d[irank]
}
mat.cl = FilterPlus(mat.cl)

cube.org = array(mat, dim=c(npos.x, npos.y, npos.t))
cube.cl  = array(mat.cl, dim=c(npos.x, npos.y, npos.t))
file.org = sprintf("%s/%s_org.fits", outdir, outfile.head)
file.cl  = sprintf("%s/%s_cl.fits", outdir, outfile.head)
writeFITSim(cube.org, file=file.org)
writeFITSim(cube.cl, file=file.cl)

res = nmf(mat.cl, nrank, seed = "nndsvd")
base = basis(res)
coef = coef(res)

cube.img = array(base, dim=c(npos.x, npos.y, nrank))
image.comp = array(coef, dim=c(nrank, npos.t))

file.img = sprintf("%s/%s_img.fits", outdir, outfile.head)
file.comp = sprintf("%s/%s_spec.fits", outdir, outfile.head)
writeFITSim(cube.img, file=file.img)
writeFITSim(image.comp, file=file.comp)


ene.lo = 0.5
ene.up = 7.7
bin.vec = GetBin(npos.t, ene.lo, ene.up, "lin")
ene.serr = (ene.up - ene.lo) / npos.t / 2.

file.spec = sprintf("%s/%s_spec.qdp",
    outdir, outfile.head)
sink(file.spec)
printf("skip sing\n")
printf("\n")

for(irank in 1:nrank){
    printf("! irank = %d\n", irank)
    for(itime in 1:npos.t){
        ene = bin.vec[itime]
        printf("%e %e\n", ene, coef[irank, itime])
    }
    printf("\n")
    printf("no\n")
    printf("\n")
}
sink()

### residual
mat.rep = base %*% coef
diff.mat = mat - mat.rep
diff.norm = norm(diff.mat, type=c("F")) / norm(mat, type=c("F"))

file.diff = sprintf("%s/%s_diff_r%2.2d.dat",
    outdir, outfile.head, nrank)
sink(file.diff)
printf("%d  %e\n", nrank, diff.norm)
sink()

##### time ed
time.ed = Sys.time()
as.difftime(time.ed - time.st)

