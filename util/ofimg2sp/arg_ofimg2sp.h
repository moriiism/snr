#ifndef MORIIISM_SNR_UTIL_OFIMG2SP_ARG_OFIMG2SP_H_
#define MORIIISM_SNR_UTIL_OFIMG2SP_ARG_OFIMG2SP_H_

#include "mi_base.h"

class ArgValOfimg2sp : public MiArgBase{
public:
    ArgValOfimg2sp() :
        MiArgBase(),
        progname_(""),
        in_file1_(""),
        in_file2_(""),
        subimg_dat_(""),
        src_list_(""),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValOfimg2sp(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInFile1() const {return in_file1_;};
    string GetInFile2() const {return in_file2_;};
    string GetSubimgDat() const {return subimg_dat_;};
    string GetSrcList() const {return src_list_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string in_file1_;
    string in_file2_;
    string subimg_dat_;
    string src_list_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_SNR_UTIL_OFIMG2SP_ARG_OFIMG2SP_H_
