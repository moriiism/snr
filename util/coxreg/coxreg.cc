#include "mi_str.h"
#include "mi_iolib.h"
#include "mir_hist2d_nerr.h"
#include "mir_qdp_tool.h"

#include "arg_coxreg.h"

#include "TCanvas.h"
#include "TH1D.h"


// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValCoxreg* argval = new ArgValCoxreg;
    argval->Init(argc, argv);
    argval->Print(stdout);

    char logfile[kLineSize];
    if( MiIolib::TestFileExist(argval->GetOutdir()) ){
        char cmd[kLineSize];
        sprintf(cmd, "mkdir -p %s", argval->GetOutdir().c_str());
        system(cmd);
    }
    sprintf(logfile, "%s/%s_%s.log",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str(),
            argval->GetProgname().c_str());
    FILE* fp_log = fopen(logfile, "w");
    MiIolib::Printf2(fp_log, "-----------------------------\n");
    argval->Print(fp_log);

    long nline = 0;
    string* lines_arr = NULL;
    MiIolib::GenReadFileSkipComment(argval->GetInFile(),
                           &lines_arr, &nline);
    int nsplit = 0; 
    string* split_arr = NULL;
    for(long iline = 0; iline < nline; iline ++){
        if( "polygon" == lines_arr[iline].substr(0, 7)){
            MiStr::GenSplit(lines_arr[iline].
                            substr(8, lines_arr[iline].length() - 9),
                            &nsplit, &split_arr, ",");
        }
    }
    MiIolib::DelReadFile(lines_arr);

    int npoint = nsplit / 2;
    double* xpoint_arr = new double [npoint];
    double* ypoint_arr = new double [npoint];
    for(int ipoint = 0; ipoint < npoint; ipoint ++){
        xpoint_arr[ipoint] = atof(split_arr[ipoint * 2].c_str());
        ypoint_arr[ipoint] = atof(split_arr[ipoint * 2 + 1].c_str());
    }
    MiStr::DelSplit(split_arr);
    
    for(int ipoint = 0; ipoint < npoint; ipoint ++){
        printf("%e, %e\n",
               xpoint_arr[ipoint],
               ypoint_arr[ipoint]);
    }

    GraphDataNerr2d* gd2d = new GraphDataNerr2d;
    gd2d->Init(npoint);
    gd2d->SetXvalArr(npoint, xpoint_arr);
    gd2d->SetOvalArr(npoint, ypoint_arr);
    MirQdpTool::MkQdp(gd2d, "temp.qdp", "x,y");
    delete gd2d;
    
    HistDataNerr2d* hd2d = new HistDataNerr2d;
    hd2d->Init(100, 0, 10, 100, 0, 10);


    
//    TCanvas* can = new TCanvas;
//    TH1D* th1d = new TH1D("aaa", "aaa", 100, 0, 100);
//    th1d->Draw();
//    can->Print("aaa.png");
    
    return status_prog;
}
