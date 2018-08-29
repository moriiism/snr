#ifndef MORIIISM_SNR_UTIL_OPTFLOW_SUB_H_
#define MORIIISM_SNR_UTIL_OPTFLOW_SUB_H_

#include "mi_base.h"

#include "mi_str.h"
#include "mi_iolib.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_hist2d_nerr.h"
#include "mir_data1d_ope.h"

#include "TArrow.h"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>

using namespace cv;

void DrawVectMap(int posx_c, int posy_c,
                 const Mat& flow, const Mat& img,
                 MirRootTool* root_tool,
                 string outdir, string outfile_head);

void DrawFlowVelMap(int posx_c, int posy_c,
                    const cv::Mat& flow, const cv::Mat& img,
                    int bitpix,
                    MirRootTool* root_tool,
                    string outdir, string outfile_head);

#endif // MORIIISM_SNR_UTIL_OPTFLOW_SUB_H_

