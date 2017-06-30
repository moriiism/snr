#include "sub.h"

void DrawVectPyrLKMap(vector<cv::Point2f> points,
                      vector<cv::Point2f> points_next,
                      const cv::Mat& img,
                      MirRootTool* root_tool,
                      string tag)
{
    HistDataNerr2d* hd2d = new HistDataNerr2d;
    hd2d->Init(img.cols, 0, img.cols, img.rows, 0, img.rows);
    for(long ibinx = 0; ibinx < img.cols; ibinx ++){
        for(long ibiny = 0; ibiny < img.rows; ibiny ++){
            long index = ibinx + ibiny * img.cols;
            hd2d->SetOvalElm(ibinx, ibiny, img.at<signed char>(ibiny, ibinx));
        }
    }
    TH2D* th2d = hd2d->GenTH2D(0.0, 0.0, 0.0);
    th2d->Draw("colz");

    for(int index = 0; index < points.size(); index ++){
        printf("%e, %e\n", points[index].x, points[index].y);
        TArrow* arrow = new TArrow(points[index].x, points[index].y, points_next[index].x, points_next[index].y);
        arrow->SetArrowSize(0.01);
        arrow->SetLineColor(4);
        arrow->SetLineWidth(3);
        arrow->Draw();
    }

    root_tool->GetTCanvas()->SetCanvasSize(1200, 1200);
    char outfile[kLineSize];
    sprintf(outfile, "sparse_%s.png", tag.c_str());
    root_tool->GetTCanvas()->Print(outfile);
    root_tool->GetTCanvas()->Clear();
}










void DrawVectMap(const cv::Mat& flow, const cv::Mat& img,
                 MirRootTool* root_tool,
                 string tag)
{
    HistDataNerr2d* hd2d = new HistDataNerr2d;
    hd2d->Init(img.cols, 0, img.cols, img.rows, 0, img.rows);
    for(long ibinx = 0; ibinx < img.cols; ibinx ++){
        for(long ibiny = 0; ibiny < img.rows; ibiny ++){
            long index = ibinx + ibiny * img.cols;
            hd2d->SetOvalElm(ibinx, ibiny, img.at<signed char>(ibiny, ibinx));
        }
    }
    TH2D* th2d = hd2d->GenTH2D(0.0, 0.0, 0.0);
    th2d->Draw("colz");

    printf("draw vec map\n");
    
    int step = 10;
    for(int posy = 0; posy < flow.rows; posy += step) {
        for(int posx = 0; posx < flow.cols; posx += step) {
            const Point2f& fxy = flow.at<Point2f>(posy, posx);

            if(pow(fxy.x,2) + pow(fxy.y, 2) < 2){
                continue;
            }
            // TArrow* arrow = new TArrow(posx, posy, posx + fxy.x * 1e15, posy + fxy.y * 1e15);
            TArrow* arrow = new TArrow(posx, posy, posx + fxy.x * 10, posy + fxy.y * 10);

            
            arrow->SetArrowSize(0.01);
            arrow->SetLineColor(2);
            arrow->SetLineWidth(3);
            arrow->Draw();
        }
    }
    root_tool->GetTCanvas()->SetCanvasSize(1200, 1200);
    char outfile[kLineSize];
    sprintf(outfile, "vect_%s.png", tag.c_str());
    root_tool->GetTCanvas()->Print(outfile);
    root_tool->GetTCanvas()->Clear();
}

//void DrawFlowVelMap(const cv::Mat& flow, const cv::Mat& img,
//                    int bitpix,
//                    MirRootTool* root_tool)
//{
//
//    HistDataNerr2d* hd2d = new HistDataNerr2d;
//    hd2d->Init(img.cols, 0, img.cols, img.rows, 0, img.rows);
//    for(long ibinx = 0; ibinx < img.cols; ibinx ++){
//        for(long ibiny = 0; ibiny < img.rows; ibiny ++){
//            long index = ibinx + ibiny * img.cols;
//            const Point2f& fxy = flow.at<Point2f>(ibiny, ibinx);
//
//            //double r2_o = pow(ibinx + fxy.x - posx_c, 2) + pow(ibiny + fxy.y - posy_c, 2);
//            //double r2_i = pow(ibinx - posx_c, 2) + pow(ibiny - posy_c, 2);
//            hd2d->SetOvalElm(ibinx, ibiny, r2_o - r2_i);
//        }
//    }
//    TH2D* th2d = hd2d->GenTH2D(0.0, 0.0, 0.0);
//
////    int colors[] = {2, 3, 4, 5, 6, 7, 8, 9, 10}; // #colors >= #levels - 1
////    gStyle->SetPalette((sizeof(colors)/sizeof(int)), colors);
////    // #levels <= #colors + 1 (notes: +-1.79e308 = +-DBL_MAX; +1.17e-38 = +FLT_MIN)
////    double levels[] = {-400, -300, -200, -100, 0.0, 100, 200, 300, 400, 500};
////    th2d->SetContour((sizeof(levels)/sizeof(double)), levels);
//
//    th2d->Draw("colz");
//    root_tool->GetTCanvas()->SetCanvasSize(1200, 1200);
//    root_tool->GetTCanvas()->Print("flowvel.png");
//
//    long* naxes = new long[2];
//    naxes[0] = img.cols;
//    naxes[1] = img.rows;
//    double* data_arr = new double[img.cols * img.rows];
//    for(long iarr = 0; iarr < img.cols * img.rows; iarr ++){
//        data_arr[iarr] = hd2d->GetOvalArr()->GetValElm(iarr);
//    }
//    MifFits::OutFitsImageD(".", "casa", "flowvel", 2,
//                           bitpix,
//                           naxes,
//                           data_arr);
//    delete [] naxes;
//    delete [] data_arr;
//    
//    delete th2d;
//}
