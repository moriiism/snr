#include "fitsio.h"
#include "mi_str.h"
#include "mi_iolib.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_hist2d_nerr.h"
#include "mir_data1d_ope.h"
#include "arg_ofimg2.h"
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
    
    ArgValOfimg2* argval = new ArgValOfimg2;
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
    // rescale frames
    double mat1_max = MirMath::GetMax(img_info_in->GetNpixelImg(), img_mat1);
    Mat mat1(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8SC1);
    Mat mat1_plus(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8UC1);
    for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny++){
        for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx++){
            long index = ibiny * img_info_in->GetNaxesArrElm(0) + ibinx;
            double val = img_mat1[index] / mat1_max * 127;
            if(val < -128 ){
                val = -128;
            } else if(val > 127 ){
                val = 127;
            }
            mat1.at<signed char>(ibiny, ibinx) =  (signed char) val;

            double val_plus = img_mat1[index] / mat1_max * 255;
            if(val <= 0.0 ){
                val_plus = 0.0;
            } else if(val > 255 ){
                val_plus = 255;
            }
            mat1_plus.at<unsigned char>(ibiny, ibinx) =  (unsigned char) val_plus;
        }
    }


    double mat2_max = MirMath::GetMax(img_info_in->GetNpixelImg(), img_mat2);
    Mat mat2(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8SC1);
    Mat mat2_plus(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8UC1);
    for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny++){
        for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx++){
            long index = ibiny * img_info_in->GetNaxesArrElm(0) + ibinx;
            double val = img_mat2[index] / mat2_max * 127;
            if(val < -128 ){
                val = -128;
            } else if(val > 127 ){
                val = 127;
            }
            mat2.at<signed char>(ibiny, ibinx) =  (signed char) val;

            double val_plus = img_mat2[index] / mat2_max * 255;
            if(val <= 0.0 ){
                val_plus = 0.0;
            } else if(val > 255 ){
                val_plus = 255;
            }
            mat2_plus.at<unsigned char>(ibiny, ibinx) =  (unsigned char) val_plus;
        }
    }

    Mat flow(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_64FC2);
    calcOpticalFlowFarneback(mat1_plus, mat2_plus, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    int posx_c = 229;
    int posy_c = 229;
    
    DrawVectMap(posx_c, posy_c, flow, mat1, root_tool,
                argval->GetOutdir(), argval->GetOutfileHead());
    DrawFlowVelMap(posx_c, posy_c, flow, mat1, bitpix, root_tool,
                   argval->GetOutdir(), argval->GetOutfileHead());
    
    return status_prog;
}



