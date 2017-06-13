#include "fitsio.h"
#include "mi_str.h"
#include "mi_iolib.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_hist2d_nerr.h"
#include "mir_data1d_ope.h"
#include "arg_optflow.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;



static void help()
{
    cout <<
        "\nThis program demonstrates dense optical flow algorithm by Gunnar Farneback\n"
        "Mainly the function: calcOpticalFlowFarneback()\n"
        "Call:\n"
        "./fback\n"
        "This reads from video camera 0\n" << endl;
}
static void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step,
                           const Scalar& color)
{
    //circle(cflowmap, Point(221,239), 2, color, -1);

    int x_c = 221;
    int y_c = 239;
    
    for(int y = 0; y < cflowmap.rows; y += step)
        for(int x = 0; x < cflowmap.cols; x += step)
            {
                const Point2f& fxy = flow.at<Point2f>(y, x);

                double r2_o = pow(x+fxy.x - x_c, 2) + pow(y+fxy.y - y_c, 2);
                double r2_i = pow(x - x_c, 2) + pow(y - y_c, 2);
                if( r2_o - r2_i < 0.0){
                    line(cflowmap, Point(x,y), Point(cvRound(x+fxy.x), cvRound(y+fxy.y)),
                         color);
                    // circle(cflowmap, Point(x,y), 2, color, -1);
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
    
//    ArgValOptflow* argval = new ArgValOptflow;
//    argval->Init(argc, argv);
//    argval->Print(stdout);
//
//    char logfile[kLineSize];
//    if( MiIolib::TestFileExist(argval->GetOutdir()) ){
//        char cmd[kLineSize];
//        sprintf(cmd, "mkdir -p %s", argval->GetOutdir().c_str());
//        system(cmd);
//    }
//    sprintf(logfile, "%s/%s_%s.log",
//            argval->GetOutdir().c_str(),
//            argval->GetOutfileHead().c_str(),
//            argval->GetProgname().c_str());
//    FILE* fp_log = fopen(logfile, "w");
//    MiIolib::Printf2(fp_log, "-----------------------------\n");
//    argval->Print(fp_log);
//
//    MirRootTool* root_tool = new MirRootTool;
//    root_tool->InitTCanvas("pub");


    //cv::CommandLineParser parser(argc, argv, "{help h||}");
//    if (parser.has("help"))
//        {
//            help();
//            return 0;
//        }
    // VideoCapture cap(0);

    Mat img1 = cv::imread("/home/morii/work/snr/ana/ds9_1.png", cv::IMREAD_GRAYSCALE);
    Mat img2 = cv::imread("/home/morii/work/snr/ana/ds9_9.png", cv::IMREAD_GRAYSCALE);
    std::cout << "width:"     << img1.cols << ' ';
    std::cout << "height:"    << img1.rows << ' ';
    std::cout << "steps:"     << img1.step << ' ';


    std::cout << "width:"     << img2.cols << ' ';
    std::cout << "height:"    << img2.rows << ' ';
    std::cout << "steps:"     << img2.step << ' ';    
    
    //VideoCapture cap("/home/morii/work/snr/ana/ds9.mpeg");
    //help();
    //.if( !cap.isOpened() )
    //    return -1;
    Mat flow, cflow, frame;
    UMat gray, prevgray, uflow;
    
    namedWindow("flow", 1);

    UMat gray1, gray2;

    printf("jjjj\n");

    //cvtColor(img1, gray1, COLOR_BGR2GRAY);

    printf("kkkk\n");
  
    //cvtColor(img2, gray2, COLOR_BGR2GRAY);

    printf("jjjj\n");

    // CvMat* flow = cvCreateMat(prev->height, next->width, CV_32FC2);

    calcOpticalFlowFarneback(img1, img2, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    printf("dddd\n");

    img1.copyTo(cflow);
    
    // uflow.copyTo(flow);
    printf("iiiii\n");

    //imshow("flow", flow);
    
    drawOptFlowMap(flow, cflow, 5, Scalar(255, 255, 255));
    imshow("flow", cflow);
    
    char outfile[kLineSize];
    sprintf(outfile, "out.png");
    printf("%s\n", outfile);
    imwrite(outfile, cflow);

    printf("jjjjjjjjj\n");
    
    
//    
//    char outfile[kLineSize];
//    sprintf(outfile, "out.png");
//    printf("%s\n", outfile);
//    imwrite(outfile, frame);
//
//    abort();
//
//    int index = 0;
//    for(;;)
//        {
//            //cap >> frame;
//            printf("jjjj\n");
//            cvtColor(frame, gray, COLOR_BGR2GRAY);
//            //if( !prevgray.empty() )
//                {
//                    index ++;
//
//                    printf("ssss\n");
//                    
//                    calcOpticalFlowFarneback(prevgray, gray, uflow, 0.5, 3, 15, 3, 5, 1.2, 0);
//                    cvtColor(prevgray, cflow, COLOR_GRAY2BGR);
//
//                    printf("vvvv\n");
//                    
//                    uflow.copyTo(flow);
//                    drawOptFlowMap(flow, cflow, 16, 1.5, Scalar(0, 255, 0));
//                    // imshow("flow", cflow);
//                    char outfile[kLineSize];
//                    sprintf(outfile, "out_%2.2d.png", index);
//                    printf("%s\n", outfile);
//                    imwrite(outfile, cflow);
//                }
//            if(waitKey(30)>=0)
//                break;
//            std::swap(prevgray, gray);
//
//            
//        }
    
    return status_prog;
}



