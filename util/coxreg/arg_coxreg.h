#ifndef MORIIISM_SNR_UTIL_COXREG_ARG_COXREG_H_
#define MORIIISM_SNR_UTIL_COXREG_ARG_COXREG_H_

#include "mi_base.h"

class ArgValCoxreg : public MiArgBase{
public:
    ArgValCoxreg() :
        MiArgBase(),
        progname_(""),
        in_file_(""),
        ntheta_(0),
        npix_(0),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValCoxreg(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInFile() const {return in_file_;};
    int    GetNtheta() const {return ntheta_;};
    long   GetNpix() const {return npix_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string in_file_;
    int    ntheta_;
    long   npix_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_SNR_UTIL_COXREG_ARG_COXREG_H_
