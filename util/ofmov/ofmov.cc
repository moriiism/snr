#include "fitsio.h"
#include "mi_str.h"
#include "mi_iolib.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_hist2d_nerr.h"
#include "mir_data1d_ope.h"
#include "arg_ofmov.h"
#include "sub.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
//#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"

//#include "opencv2/features2d/features2d.hpp"
#include <opencv2/features2d/features2d.hpp>

#include "opencv2/features2d.hpp"
#include "opencv2/objdetect.hpp"

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
    
    ArgValOfmov* argval = new ArgValOfmov;
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
    int bitpix = 0;
    double* cube_mat = new double [npix_cube];
    MifFits::InFitsCubeD(argval->GetInFile(),
                         img_info_in,
                         &bitpix,
                         &cube_mat);
    double mat_max = MirMath::GetMax(img_info_in->GetNpixelTotal(), cube_mat);
    double mat_min = MirMath::GetMin(img_info_in->GetNpixelTotal(), cube_mat);
    printf("mat_min = %e\n", mat_min);
    printf("mat_max = %e\n", mat_max);

    // rescale frames
    double* mat_max_arr = new double [img_info_in->GetNframe()];
    double* mat_min_arr = new double [img_info_in->GetNframe()];
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        mat_max_arr[iframe] = MirMath::GetMax(img_info_in->GetNpixelImg(),
                                              cube_mat + iframe * img_info_in->GetNpixelImg());
        mat_min_arr[iframe] = MirMath::GetMin(img_info_in->GetNpixelImg(),
                                              cube_mat + iframe * img_info_in->GetNpixelImg());
    }

    Mat** mat_arr = new Mat* [img_info_in->GetNframe()];
    Mat** mat_plus_arr = new Mat* [img_info_in->GetNframe()];
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        mat_arr[iframe] = new Mat(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8SC1);
        mat_plus_arr[iframe] = new Mat(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_8UC1);

        for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny++){
            for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx++){
                long index = iframe * img_info_in->GetNpixelImg()
                    + ibiny * img_info_in->GetNaxesArrElm(0) + ibinx;

                double val = cube_mat[index] / mat_max_arr[iframe] * 128;
                if(val < -128 ){
                    val = -128;
                } else if(val > 127 ){
                    val = 127;
                }
                //(signed char) (cube_mat[index] / 1.e-3 * 128);
                mat_arr[iframe]->at<signed char>(ibiny, ibinx) =  (signed char) val;

                double val_plus = cube_mat[index] / mat_max_arr[iframe] * 256;
                if(val <= 0.0 ){
                    val_plus = 0.0;
                } else if(val > 255 ){
                    val_plus = 255;
                }
                mat_plus_arr[iframe]->at<unsigned char>(ibiny, ibinx) =  (unsigned char) val_plus;
                
                // printf("%e  %d\n", cube_mat[index]/1e-3 * 128, mat_arr[iframe]->at<signed char>(ibiny, ibinx));
                
            }
        }

        printf("iframe = %ld\n", iframe);
        
    }


    Mat** flow_arr = new Mat* [img_info_in->GetNframe() - 1];
    for(long iframe = 0; iframe < img_info_in->GetNframe() - 1; iframe ++){
        flow_arr[iframe] = new Mat(img_info_in->GetNaxesArrElm(0), img_info_in->GetNaxesArrElm(1), CV_32FC2);
    }

    vector<cv::Point2f>* points_arr = new vector<cv::Point2f> [img_info_in->GetNframe() - 1];
    vector<cv::Point2f>* points_next_arr = new vector<cv::Point2f> [img_info_in->GetNframe() - 1];

    for(long iframe = 0; iframe < img_info_in->GetNframe() - 1; iframe ++){
        //calcOpticalFlowFarneback(*mat_arr[iframe], *mat_arr[iframe+1],
        //                         *flow_arr[iframe], 0.5, 3, 15, 3, 5, 1.2, 0);
        calcOpticalFlowFarneback(*mat_plus_arr[iframe], *mat_plus_arr[iframe+1],
                                 *flow_arr[iframe], 0.5, 3, 15, 3, 5, 1.2, 0);

        //calcOpticalFlowSparseToDense(*mat_plus_arr[iframe], *mat_plus_arr[iframe+1],
        //                             *flow_arr[iframe]);
        //int  	grid_step = 8,
        //int  	k = 128,
        //float  	sigma = 0.05f,
        //bool  	use_post_proc = true,
        //float  	fgs_lambda = 500.0f,
        //float  	fgs_sigma = 1.5f 
        
        // Setup SimpleBlobDetector parameters.
        SimpleBlobDetector::Params params;
        // Change thresholds
        params.minThreshold = 0;
        // params.maxThreshold = 255;
        // Filter by Area.
        //params.filterByArea = true;
        //params.minArea = 30;
        // Filter by Circularity
//        params.filterByCircularity = true;
//        params.minCircularity = 0.1;
        // Filter by Convexity
//        params.filterByConvexity = true;
//        params.minConvexity = 0.87;
//        // Filter by Inertia
//        params.filterByInertia = true;
//        params.minInertiaRatio = 0.01;


        
        
        
        cv::Mat descriptors;
        std::vector<cv::KeyPoint> keypoints;
        //cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params);
        cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create();
        detector->detect(*mat_plus_arr[iframe], keypoints, cv::noArray());
        printf("keypoints.size() = %d\n", keypoints.size());

        //-- Step 1: Detect the keypoints using SURF Detector
        //int minHessian = 400;
        //Ptr<SURF> detector = SURF::create( minHessian );
        //detector->detect( *mat_plus_arr[iframe], keypoints);

        for( vector<cv::KeyPoint>::iterator itk = keypoints.begin(); itk != keypoints.end(); ++itk){
            points_arr[iframe].push_back(itk->pt);
        }

        vector<uchar> status;
        vector<float> err;
        
//        calcOpticalFlowPyrLK(*mat_plus_arr[iframe], *mat_plus_arr[iframe+1],
//                             points_arr[iframe], points_next_arr[iframe], status, err);
    }

    for(long iframe = 0; iframe < img_info_in->GetNframe() - 1; iframe ++){
        char tag[kLineSize];
        sprintf(tag, "%2.2ld", iframe);
        DrawVectMap(*flow_arr[iframe], *mat_arr[iframe], root_tool, tag);

        // DrawVectPyrLKMap(points_arr[iframe], points_next_arr[iframe], *mat_plus_arr[iframe], root_tool, tag);

        
    }

    
    return status_prog;
}



