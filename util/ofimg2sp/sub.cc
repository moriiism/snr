#include "sub.h"

void DrawVectPyrLKMap(vector<cv::Point2f> points,
                      vector<cv::Point2f> points_next,
                      const cv::Mat& img,
                      MirRootTool* root_tool,
                      string outdir,
                      string outfile_head)
{
    HistDataNerr2d* hd2d = new HistDataNerr2d;
    hd2d->Init(img.cols, 0, img.cols, img.rows, 0, img.rows);
    for(long ibinx = 0; ibinx < img.cols; ibinx ++){
        for(long ibiny = 0; ibiny < img.rows; ibiny ++){
            long index = ibinx + ibiny * img.cols;
            hd2d->SetOvalElm(ibinx, ibiny, img.at<unsigned char>(ibiny, ibinx));
        }
    }
    TH2D* th2d = hd2d->GenTH2D(0.0, 0.0, 0.0);
    th2d->Draw("colz");

    for(int index = 0; index < points.size(); index ++){
        printf("%e, %e\n", points[index].x, points[index].y);
        TArrow* arrow = new TArrow(points[index].x, points[index].y, points_next[index].x, points_next[index].y);
        arrow->SetArrowSize(0.01);
        arrow->SetLineColor(1);
        arrow->SetLineWidth(3);
        arrow->Draw();
    }

    root_tool->GetTCanvas()->SetCanvasSize(1200, 1200);
    char outfile[kLineSize];
    sprintf(outfile, "%s/%s_vect.png", outdir.c_str(), outfile_head.c_str());
    root_tool->GetTCanvas()->Print(outfile);
    root_tool->GetTCanvas()->Clear();
}
