#!/usr/bin/Rscript

### #!/usr/local/bin/Rscript

###
### qp.S.R
###
### 2016.12.12 M.Morii
###

require(FITSio)
library("quadprog")

snrtooldir = "/home/morii/work/github/moriiism/snr"
source( paste(snrtooldir, "scriptR/rlib/iolib.R", sep="/") )
source( paste(snrtooldir, "scriptR/rlib/binning.R", sep="/") )
source( paste(snrtooldir, "scriptR/rlib/vec_ope.R", sep="/") )

args <- commandArgs(TRUE)
in.file   = args[1]
subcube   = args[2]
spec.list = args[3]
outdir    = args[4]
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


### spec
spec.df <- read.table(spec.list, comment.char = "#")
print(spec.df)

spec.init.mat = matrix(0.0, nrow = nrow(spec.df), ncol = npos.t)
for(ispec in 1:nrow(spec.df)){
    printf("%s\n", spec.df[ispec,1])
    spec.this.df <- read.table(as.character(spec.df[ispec,1]),
                               comment.char = "#", skip=2)
    for(ibin in 1:nrow(spec.this.df)){
        spec.init.mat[ispec, ibin] = spec.this.df[ibin, 3]
    }
}
printf("nrow(spec.init.mat) = %d\n", nrow(spec.init.mat))
printf("ncol(spec.init.mat) = %d\n", ncol(spec.init.mat))

nspec = nrow(spec.df)

### time st
time.st = Sys.time()

mat =  matrix(
    as.vector(
        fits$imDat[pos.x.first:pos.x.last,
                   pos.y.first:pos.y.last,
                   pos.t.first:pos.t.last]),
    nrow=nrow, ncol=ncol)
rm(fits)

### for QP

H.mat = spec.init.mat


img.mat = matrix(0.0, nrow=nrow, ncol=nspec)

### loop for pixel
for(irow in 1:nrow){
    ## printf("irow = %d / nrow(%d)\n", irow, nrow)

    V.vec = mat[irow,]
    V.tmp1.vec = sqrt( V.vec + 1)
    V.tmp2.vec = (V.vec + min(V.vec, 1)) / (V.vec + 1)
    H1.mat = H.mat
    H2.mat = H.mat
    for(ispec in 1: nrow(H.mat)){
        H1.mat[ispec,] = H.mat[ispec,] / V.tmp1.vec
        H2.mat[ispec,] = H.mat[ispec,] * V.tmp2.vec
    }
    
    D.mat = H1.mat %*% t(H1.mat)
    d.vec = apply(H2.mat, 1, sum)
    A.mat = diag(nspec)
    b0.vec = rep(0.0, nspec)

###    print(head(D.mat))
###    print(head(d.vec))
###    print(head(A.mat))
###    print(head(b0.vec))

    ret.qp = solve.QP(D.mat, d.vec, A.mat, b0.vec, meq=0, factorized=FALSE)

### print(ret.qp$solution)
    img.mat[irow,] = ret.qp$solution
}

### residual
mat.rep = img.mat %*% H.mat
diff.mat = mat - mat.rep
diff.norm = norm(diff.mat, type=c("F")) / norm(mat, type=c("F"))

file.diff = sprintf("%s/%s_diff.dat",
    outdir, outfile.head)
sink(file.diff)
printf("%e\n", diff.norm)
sink()

cube.org = array(mat, dim=c(npos.x, npos.y, npos.t))
cube.rep = array(mat.rep, dim=c(npos.x, npos.y, npos.t))
cube.img = array(img.mat, dim=c(npos.x, npos.y, nspec))
image.comp = array(H.mat, dim=c(nspec, npos.t))
cube.diff = array(diff.mat, dim=c(npos.x, npos.y, npos.t))

file.org = sprintf("%s/%s_org.fits", outdir, outfile.head)
file.rep = sprintf("%s/%s_rep.fits", outdir, outfile.head)
file.img = sprintf("%s/%s_img.fits", outdir, outfile.head)
file.comp = sprintf("%s/%s_spec.fits", outdir, outfile.head)
file.diff = sprintf("%s/%s_diff.fits", outdir, outfile.head)
writeFITSim(cube.org, file=file.org)
writeFITSim(cube.rep, file=file.rep)
writeFITSim(cube.img, file=file.img)
writeFITSim(image.comp, file=file.comp)
writeFITSim(cube.diff, file=file.diff)

### reproduced each component

for(ispec in 1:nspec){
    mat.rep.each = matrix(img.mat[,ispec], nrow=nrow(img.mat), ncol=ncol(img.mat)) %*%
                   matrix(H.mat[ispec,], nrow=nrow(H.mat), ncol=ncol(H.mat))
    ### cube.rep.each = array(mat.rep.each, dim=c(npos.x, npos.y, npos.t))
    ### file.rep.each = sprintf("%s/%s_rep_%2.2d.fits", outdir, outfile.head, ispec)
    ### writeFITSim(cube.rep.each, file=file.rep.each)
    
    mat.rep.sum = apply(mat.rep.each, 1, sum)
    image.rep.sum = array(mat.rep.sum, dim=c(npos.x, npos.y))
    file.rep.sum.each = sprintf("%s/%s_rep_%2.2d.fits", outdir, outfile.head, ispec)
    writeFITSim(image.rep.sum, file=file.rep.sum.each)
}


##### time ed
time.ed = Sys.time()
as.difftime(time.ed - time.st)

