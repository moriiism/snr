###
### nmf.mat
###
### 2015.06.12 M.Morii
### 2015.09.04 M.Morii
###

nmf.mat <- function(X,
                    nrow,
                    ncol,
                    rank,
                    outdir){
    res = nmf(X, rank)
    base = basis(res)
    coef = coef(res)

    dir.create(outdir, recursive = TRUE)

    ## base matrix(nrow, rank)
    for(irank in 1 : rank){
        basis.file = sprintf("%s/basis_%2.2d.dat",
            outdir, irank)
        sink(basis.file)
        for(irow in 1 : nrow){
            printf("%d  %e\n",
                   irow, base[irow, irank])
        }
        sink()
    }

    ## coeff matrix(rank, ncol)
    for(irank in 1 : rank){
        coeff.file = sprintf("%s/coeff_%2.2d.dat",
            outdir, irank)
        sink(coeff.file)
        for(icol in 1 : ncol){
            printf("%d  %e\n", icol, coef[irank, icol])
        }
        sink()
    }

    return(res)
}
