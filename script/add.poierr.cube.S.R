#!/usr/local/bin/Rscript
### #!/opt/local/bin/Rscript

###
### add.poierr.S.R
###
### 2016.12.09 M.Morii
###

require(FITSio)
library("quadprog")

snrtooldir = "/home/morii/work/github/moriiism/snr"
### snrtooldir = "/Users/katsuda/work/morii/snr"
source( paste(snrtooldir, "scriptR/rlib/iolib.R", sep="/") )
source( paste(snrtooldir, "scriptR/rlib/binning.R", sep="/") )
source( paste(snrtooldir, "scriptR/rlib/vec_ope.R", sep="/") )

args <- commandArgs(TRUE)
in.file   = args[1]
outdir    = args[2]
outfile.head = args[3]
nfile     = as.integer(args[4])
nran      = as.integer(args[5])

dir.create(outdir, recursive = TRUE)
fits <- readFITS(in.file)

### time st
time.st = Sys.time()

nx = length(fits$imDat[,1,1])
ny = length(fits$imDat[1,,1])
nt = length(fits$imDat[1,1,])

printf("nx = %d\n", nx)
printf("ny = %d\n", ny)
printf("nt = %d\n", nt)

nrow = nx * ny
ncol = nt

mat = matrix(as.vector(fits$imDat),
    nrow=nrow, ncol=ncol)
rm(fits)

array.adderr = array(0.0, dim=c(nrow, ncol, nran))

nloop = nfile / nran
ifile = 0
for(iloop in 1:nloop){
    printf("iloop = %d\n", iloop)
    ## loop for pixel
    for(irow in 1:nrow){
        ## printf("irow / nrow = %d / %d\n", irow, nrow)
        ## loop for spec
        for(icol in 1:ncol){
            adderr.vec = rpois(nran, lambda = mat[irow, icol])
            ##if(mat[irow, icol] > 0){
            ##    printf("org = %e, adderr = %e\n",
            ##           mat[irow, icol], adderr[1])
            ##}
            array.adderr[irow, icol,] = adderr.vec
        }
    }
    for(iran in 1:nran){
        ifile = ifile + 1
        cube.out = array(array.adderr[,,iran], dim=c(nx, ny, nt))
        file.out = sprintf("%s/%s_adderr_%5.5d.fits",
            outdir, outfile.head, ifile)
        writeFITSim(cube.out, file=file.out)
    }
    time.now = Sys.time()
    print(as.difftime(time.now - time.st))
}


##### time ed
time.ed = Sys.time()
print(as.difftime(time.ed - time.st))



