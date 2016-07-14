#ifndef MORIIISM_SNR_UTIL_IMG2CUBE_ARG_IMG2CUBE_H_
#define MORIIISM_SNR_UTIL_IMG2CUBE_ARG_IMG2CUBE_H_

#include "mi_base.h"

class ArgValImg2cube : public MiArgBase{
public:
    ArgValImg2cube(string title = "") :
        MiArgBase("ArgValImg2cube", title),
        progname_(""),
        in_list_(""),
        subimg_dat_(""),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValImg2cube(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInList() const {return in_list_;};
    string GetSubimgDat() const {return subimg_dat_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string in_list_;
    string subimg_dat_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_SNR_UTIL_IMG2CUBE_ARG_IMG2CUBE_H_

