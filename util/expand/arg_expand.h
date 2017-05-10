#ifndef MORIIISM_SNR_UTIL_EXPAND_ARG_EXPAND_H_
#define MORIIISM_SNR_UTIL_EXPAND_ARG_EXPAND_H_

#include "mi_base.h"

class ArgValExpand : public MiArgBase{
public:
    ArgValExpand() :
        MiArgBase(),
        progname_(""),
        in_file_(""),
        subcube_dat_(""),
        obsmjd_dat_(""),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValExpand(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInFile() const {return in_file_;};
    string GetSubcubeDat() const {return subcube_dat_;};
    string GetObsmjdDat() const {return obsmjd_dat_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string in_file_;
    string subcube_dat_;
    string obsmjd_dat_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_SNR_UTIL_EXPAND_ARG_EXPAND_H_
