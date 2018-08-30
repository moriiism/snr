#include "sub.h"

void Dwt(const double* const img_mat, long npix_img,
         const MifImgInfo* const img_info_in,
         const double* const filter_arr, int nfil,
         int nstep,
         double** const img_dwt_mat_arr)
{
    for(int istep = 0; istep < nstep - 1; istep ++){

        for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny++){
            for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx++){

                long index0 = ibiny * img_info_in->GetNaxesArrElm(0) + ibinx;
                img_dwt_mat_arr[istep + 1][index0] = 0.0;
                for(int ifilx = 0; ifilx < nfil; ifilx ++){
                    for(int ifily = 0; ifily < nfil; ifily ++){
                        int index_x = ibinx + (ifilx - 2) * pow(2, istep);
                        int index_y = ibiny + (ifily - 2) * pow(2, istep);
                        if(index_x < 0){
                            index_x = 0;
                        }
                        if(index_x > img_info_in->GetNaxesArrElm(0)){
                            index_x = img_info_in->GetNaxesArrElm(0) - 1;
                        }                        
                        if(index_y < 0){
                            index_y = 0;
                        }
                        if(index_y > img_info_in->GetNaxesArrElm(1)){
                            index_y = img_info_in->GetNaxesArrElm(1) - 1;
                        }
                        int index = index_y * img_info_in->GetNaxesArrElm(0) + index_x;
                        img_dwt_mat_arr[istep + 1][index0] +=
                            img_dwt_mat_arr[istep][index] * filter_arr[ifilx] * filter_arr[ifily];
                    }
                }

            }
        }
        // diff
        for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny++){
            for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx++){
                long index = ibiny * img_info_in->GetNaxesArrElm(0) + ibinx;
                img_dwt_mat_arr[istep][index] -= img_dwt_mat_arr[istep + 1][index];
            }
        }
        
    }
}

void RescaleFrame(const double* const img_mat,
                  const MifImgInfo* const img_info_in,
                  Mat mat, Mat mat_plus)
{
    double mat_max = MirMath::GetMax(img_info_in->GetNpixelImg(), img_mat);
    for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny++){
        for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx++){
            long index = ibiny * img_info_in->GetNaxesArrElm(0) + ibinx;
            double val = img_mat[index] / mat_max * 127;
            if(val < -128 ){
                val = -128;
            } else if(val > 127 ){
                val = 127;
            }
            mat.at<signed char>(ibiny, ibinx) =  (signed char) val;

            double val_plus = img_mat[index] / mat_max * 255;
            if(val <= 0.0 ){
                val_plus = 0.0;
            } else if(val > 255 ){
                val_plus = 255;
            }
            mat_plus.at<unsigned char>(ibiny, ibinx) =  (unsigned char) val_plus;
        }
    }
}


void DrawVectMap(int posx_c, int posy_c,
                 const cv::Mat& flow, const cv::Mat& img,
                 MirRootTool* root_tool,
                 string outdir, string outfile_head)
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
    th2d->Draw("");

    int step = 20;
    for(int posy = 0; posy < flow.rows; posy += step) {
        for(int posx = 0; posx < flow.cols; posx += step) {
            const Point2f& fxy = flow.at<Point2f>(posy, posx);

            double inprod = (posx - posx_c) * fxy.x + (posy - posy_c) * fxy.y;
            double norm = sqrt(pow(posx - posx_c, 2) + pow(posy - posy_c, 2));

            double flow_vel = inprod / norm;

            TArrow* arrow = new TArrow(posx, posy, posx + fxy.x * 5, posy + fxy.y * 5);
            arrow->SetArrowSize(0.01);
            arrow->SetLineWidth(3);
            if(flow_vel > 0.0){
                // outflow: blue
                arrow->SetLineColor(4);
            } else {
                // inflow: red                
                arrow->SetLineColor(2);
            }
            arrow->Draw();
        }
    }
    
    root_tool->GetTCanvas()->SetCanvasSize(1200, 1200);
    char outfile[kLineSize];
    sprintf(outfile, "%s/%s_vect.png", outdir.c_str(), outfile_head.c_str());
    root_tool->GetTCanvas()->Print(outfile);
    root_tool->GetTCanvas()->Clear();
}

void DrawFlowVelMap(int posx_c, int posy_c,
                    const cv::Mat& flow, const cv::Mat& img,
                    int bitpix,
                    MirRootTool* root_tool,
                    string outdir, string outfile_head)
{
    HistDataNerr2d* hd2d = new HistDataNerr2d;
    hd2d->Init(img.cols, 0, img.cols, img.rows, 0, img.rows);
    for(long ibinx = 0; ibinx < img.cols; ibinx ++){
        for(long ibiny = 0; ibiny < img.rows; ibiny ++){
            long index = ibinx + ibiny * img.cols;
            const Point2f& fxy = flow.at<Point2f>(ibiny, ibinx);

            int posx = ibinx;
            int posy = ibiny;
            double inprod = (posx - posx_c) * fxy.x + (posy - posy_c) * fxy.y;
            double norm = sqrt(pow(posx - posx_c, 2) + pow(posy - posy_c, 2));
            double flow_vel = inprod / norm;
            hd2d->SetOvalElm(ibinx, ibiny, flow_vel);
        }
    }
    TH2D* th2d = hd2d->GenTH2D(0.0, 0.0, 0.0);

    th2d->Draw("colz");
    root_tool->GetTCanvas()->SetCanvasSize(1200, 1200);
    char outfile[kLineSize];    
    sprintf(outfile, "%s/%s_flowvel.png", outdir.c_str(), outfile_head.c_str());
    root_tool->GetTCanvas()->Print(outfile);

    long* naxes = new long[2];
    naxes[0] = img.cols;
    naxes[1] = img.rows;
    double* data_arr = new double[img.cols * img.rows];
    for(long iarr = 0; iarr < img.cols * img.rows; iarr ++){
        data_arr[iarr] = hd2d->GetOvalArr()->GetValElm(iarr);
    }
    MifFits::OutFitsImageD(outdir, outfile_head, "flowvel", 2,
                           bitpix,
                           naxes,
                           data_arr);
    delete [] naxes;
    delete [] data_arr;
    
    delete th2d;
}
