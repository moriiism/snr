#include "fitsio.h"
#include "mi_str.h"
#include "mi_iolib.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_hist2d_nerr.h"
#include "mir_data1d_ope.h"
#include "arg_optflow.h"
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


void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,
                    const Scalar& color)
{
    int x_c = 221;
    int y_c = 211;

    circle(cflowmap, Point(x_c, y_c), 2, color, -1);
    
    for(int y = 0; y < cflowmap.rows; y += step) {
        for(int x = 0; x < cflowmap.cols; x += step) {
            const Point2f& fxy = flow.at<Point2f>(y, x);
            double r2_o = pow(x+fxy.x - x_c, 2) + pow(y+fxy.y - y_c, 2);
            double r2_i = pow(x - x_c, 2) + pow(y - y_c, 2);
            if( r2_o - r2_i < 0.0){
                line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x * 10), cvRound(y+fxy.y * 10)),
                     color);
                // circle(cflowmap, Point(x,y), 2, color, -1);
            }
        }
    }
}


// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValOptflow* argval = new ArgValOptflow;
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
    img_info_in->Load(argval->GetSubcubeDat());
    img_info_in->PrintInfo();
    printf("=== img_info_in ===\n");
    
    long npix_cube = img_info_in->GetNpixelTotal();
    double* cube_mat = new double [npix_cube];
    MifFits::InFitsCubeF(argval->GetInFile(),
                         img_info_in,
                         &cube_mat);

    Mat** mat_arr = new Mat* [img_info_in->GetNframe()];
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        mat_arr[iframe] = new Mat(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_64FC(1));
        for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny++){
            for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx++){
                long index = iframe * img_info_in->GetNpixelImg()
                    + ibiny * img_info_in->GetNaxesArrElm(0) + ibinx;
                mat_arr[iframe]->at<double>(ibiny, ibinx) = cube_mat[index];
            }
        }

        char outfile[kLineSize];
        sprintf(outfile, "out_%2.2ld.png", iframe);
        printf("%s\n", outfile);
        imwrite(outfile, *mat_arr[iframe]);
    }

    
    
///    Mat img1 = cv::imread("/home/morii/work/snr/ana/ds9_1.png", cv::IMREAD_GRAYSCALE);
///    Mat img2 = cv::imread("/home/morii/work/snr/ana/ds9_9.png", cv::IMREAD_GRAYSCALE);

    Mat img1 = *mat_arr[0];
    Mat img2 = *mat_arr[8];

    HistDataNerr2d* hd2d_img1 = new HistDataNerr2d;
    hd2d_img1->Init(img_info_in->GetNaxesArrElm(0), 0, img_info_in->GetNaxesArrElm(0),
                    img_info_in->GetNaxesArrElm(1), 0, img_info_in->GetNaxesArrElm(1));
    for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx ++){
        for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny ++){
            long index = ibinx + ibiny * img_info_in->GetNaxesArrElm(0);
            
            hd2d_img1->SetOvalElm(ibinx, ibiny, img1.at<double>(ibiny, ibinx));
        }
    }
    char fig[kLineSize];
    sprintf(fig, "fig.png");
    hd2d_img1->MkTH2Fig(fig, root_tool);


    

    Mat flow, cflow, frame;
    UMat gray, prevgray, uflow;
    UMat gray1, gray2;

    calcOpticalFlowFarneback(img1, img2, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
    DrawVectMap(221, 211, flow, img1, root_tool);
    DrawFlowVelMap(221, 211, flow, img1, root_tool);

    
    
    img1.copyTo(cflow);
    for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx ++){
        for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny ++){
            cflow.at<double>(ibiny, ibinx) = cflow.at<double>(ibiny, ibinx) * 1e2;
        }
    }
    
    drawOptFlowMap(flow, cflow, 5, Scalar(255, 255, 255));


    imshow("flow", cflow);
    
    char outfile[kLineSize];
    sprintf(outfile, "out.png");
    printf("%s\n", outfile);
    imwrite(outfile, cflow);
    
    return status_prog;
}



