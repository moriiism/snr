#include "fitsio.h"
#include "mi_str.h"
#include "mi_iolib.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_hist2d_nerr.h"
#include "mir_data1d_ope.h"
#include "arg_dwt.h"
#include "sub.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValDwt* argval = new ArgValDwt;
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

    MirRootTool* root_tool = new MirRootTool;
    root_tool->InitTCanvas("pub");

    printf("--- img_info_in ---\n");
    MifImgInfo* img_info_in = new MifImgInfo;
    img_info_in->Load(argval->GetSubimgDat());
    img_info_in->PrintInfo();
    printf("=== img_info_in ===\n");
    
    long npix_img = img_info_in->GetNpixelTotal();
    int bitpix = 0;
    double* img_mat1 = new double [npix_img];
    double* img_mat2 = new double [npix_img];
    MifFits::InFitsImageD(argval->GetInFile1(),
                          img_info_in,
                          &bitpix,
                          &img_mat1);
    MifFits::InFitsImageD(argval->GetInFile2(),
                          img_info_in,
                          &bitpix,
                          &img_mat2);

    int nfil = 5;
    double* filter_arr = new double[nfil];
    filter_arr[0] = 1./16.;
    filter_arr[1] = 1./4.;
    filter_arr[2] = 3./8.;
    filter_arr[3] = 1./4.;
    filter_arr[4] = 1./16.;

    int nstep = 7;
    
    // dwt
    double** img_dwt_mat1_arr = new double* [nstep];
    for(int istep = 0; istep < nstep; istep ++){
        img_dwt_mat1_arr[istep] = new double[npix_img];
        for(int ipix = 0; ipix < npix_img; ipix ++){
            img_dwt_mat1_arr[istep][ipix] = 0.0;
        }
    }
    for(int ipix = 0; ipix < npix_img; ipix ++){
        img_dwt_mat1_arr[0][ipix] = img_mat1[ipix];
    }

    Dwt(img_mat1, npix_img, img_info_in,
        filter_arr, nfil,
        nstep, img_dwt_mat1_arr);

    for(int istep = 0; istep < nstep; istep ++){
        long* naxes = new long[2];
        naxes[0] = img_info_in->GetNaxesArrElm(0);
        naxes[1] = img_info_in->GetNaxesArrElm(1);
        char tag[kLineSize];
        sprintf(tag, "dwt1_%2.2d", istep);
        MifFits::OutFitsImageD(argval->GetOutdir(), argval->GetOutfileHead(),
                               tag, 2,
                               bitpix,
                               naxes,
                               img_dwt_mat1_arr[istep]);
        delete [] naxes;
    }

   

    // dwt
    double** img_dwt_mat2_arr = new double* [nstep];
    for(int istep = 0; istep < nstep; istep ++){
        img_dwt_mat2_arr[istep] = new double[npix_img];
        for(int ipix = 0; ipix < npix_img; ipix ++){
            img_dwt_mat2_arr[istep][ipix] = 0.0;
        }
    }
    for(int ipix = 0; ipix < npix_img; ipix ++){
        img_dwt_mat2_arr[0][ipix] = img_mat2[ipix];
    }

    Dwt(img_mat2, npix_img, img_info_in,
        filter_arr, nfil,
        nstep, img_dwt_mat2_arr);
    for(int istep = 0; istep < nstep; istep ++){
        long* naxes = new long[2];
        naxes[0] = img_info_in->GetNaxesArrElm(0);
        naxes[1] = img_info_in->GetNaxesArrElm(1);
        char tag[kLineSize];
        sprintf(tag, "dwt2_%2.2d", istep);
        MifFits::OutFitsImageD(argval->GetOutdir(), argval->GetOutfileHead(),
                               tag, 2,
                               bitpix,
                               naxes,
                               img_dwt_mat2_arr[istep]);
        delete [] naxes;
    }
    
    Mat mat1(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8SC1);
    Mat mat1_plus(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8UC1);
    RescaleFrame(img_mat1, img_info_in, mat1, mat1_plus);

    Mat mat2(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8SC1);
    Mat mat2_plus(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8UC1);
    RescaleFrame(img_mat2, img_info_in, mat2, mat2_plus);

    Mat flow(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_64FC2);
    calcOpticalFlowFarneback(mat1_plus, mat2_plus, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    int posx_c = 229;
    int posy_c = 229;
    
    DrawVectMap(posx_c, posy_c, flow, mat1, root_tool,
                argval->GetOutdir(), argval->GetOutfileHead());
    DrawFlowVelMap(posx_c, posy_c, flow, mat1, bitpix, root_tool,
                   argval->GetOutdir(), argval->GetOutfileHead());


    for(int istep = 0; istep < nstep; istep ++){
        Mat mat1(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8SC1);
        Mat mat1_plus(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8UC1);
        RescaleFrame(img_dwt_mat1_arr[istep], img_info_in, mat1, mat1_plus);

        Mat mat2(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8SC1);
        Mat mat2_plus(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8UC1);
        RescaleFrame(img_dwt_mat2_arr[istep], img_info_in, mat2, mat2_plus);

        Mat flow(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_64FC2);
        int winsize = 15 * pow(2, istep);
        calcOpticalFlowFarneback(mat1_plus, mat2_plus, flow, 0.5, 3, winsize, 3, 5, 1.2, 0);

        // InputArray prev, InputArray next,
        // InputOutputArray flow, double pyr_scale, int levels, int winsize,
        // int iterations, int poly_n, double poly_sigma, int flags );

        
        int posx_c = 229;
        int posy_c = 229;

        char outfile_head[kLineSize];
        sprintf(outfile_head, "%s_%2.2d", argval->GetOutfileHead().c_str(), istep);
        
        DrawVectMap(posx_c, posy_c, flow, mat1, root_tool,
                    argval->GetOutdir(), outfile_head);
        DrawFlowVelMap(posx_c, posy_c, flow, mat1, bitpix, root_tool,
                       argval->GetOutdir(), outfile_head);
    }
    
    
    return status_prog;
}



