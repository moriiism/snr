#ifndef MORIIISM_SNR_UTIL_OFMOV_ARG_OFMOV_H_
#define MORIIISM_SNR_UTIL_OFMOV_ARG_OFMOV_H_

#include "mi_base.h"

class ArgValOfmov : public MiArgBase{
public:
    ArgValOfmov() :
        MiArgBase(),
        progname_(""),
        in_file_(""),
        subcube_dat_(""),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValOfmov(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInFile() const {return in_file_;};
    string GetSubcubeDat() const {return subcube_dat_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string in_file_;
    string subcube_dat_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_SNR_UTIL_OFMOV_ARG_OFMOV_H_


