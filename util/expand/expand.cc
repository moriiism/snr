#include "fitsio.h"
#include "mi_str.h"
#include "mi_iolib.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_hist2d_nerr.h"
#include "mir_data1d_ope.h"
#include "arg_expand.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValExpand* argval = new ArgValExpand;
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

    
    double* obsmjd_arr = NULL;
    long nobsmjd = 0;
    printf("--- obsmjd_dat ---\n");
    MiIolib::GenReadFile(argval->GetObsmjdDat(), &obsmjd_arr, &nobsmjd);
    printf("=== obsmjd_dat ===\n");
    for(long iobs = 0; iobs < nobsmjd; iobs ++){
        printf("%ld  %e\n", iobs, obsmjd_arr[iobs]);
    }

    
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

    HistDataNerr2d** hd2d_arr = new HistDataNerr2d* [img_info_in->GetNframe()];
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        hd2d_arr[iframe] = new HistDataNerr2d;
        hd2d_arr[iframe]->Init(img_info_in->GetNaxesArrElm(0),
                               0.0,
                               img_info_in->GetNaxesArrElm(0),
                               img_info_in->GetNaxesArrElm(1),
                               0.0,
                               img_info_in->GetNaxesArrElm(1));
        hd2d_arr[iframe]->SetOvalArr(img_info_in->GetNpixelImg(),
                                     &cube_mat[iframe * img_info_in->GetNpixelImg()]);
        //        char outfig[kLineSize];
        //        sprintf(outfig, "%s/%s_%2.2d.png",
        //                argval->GetOutdir().c_str(),
        //                argval->GetOutfileHead().c_str(),
        //                iframe);
        //        hd2d_arr[iframe]->MkTH2Fig(outfig,
        //                root_tool);
    }

    HistDataNerr2d** hd2d_exp_arr = new HistDataNerr2d* [img_info_in->GetNframe()];
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        hd2d_exp_arr[iframe] = new HistDataNerr2d;
        hd2d_exp_arr[iframe]->Init(img_info_in->GetNaxesArrElm(0),
                                   0.0,
                                   img_info_in->GetNaxesArrElm(0),
                                   img_info_in->GetNaxesArrElm(1),
                                   0.0,
                                   img_info_in->GetNaxesArrElm(1));
    }

    // expand
    double x_center = 221;
    double y_center = 211;
    
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        for(long ibinx = 0; ibinx < img_info_in->GetNaxesArrElm(0); ibinx ++){
            for(long ibiny = 0; ibiny < img_info_in->GetNaxesArrElm(1); ibiny ++){

                double oval = hd2d_arr[iframe]->GetOvalElm(ibinx, ibiny);
                
                // 4 point
                double bin_center_x =
                    hd2d_arr[iframe]->GetHi2d()->GetBinCenterXFromIbinX(ibinx);
                double bin_center_y =
                    hd2d_arr[iframe]->GetHi2d()->GetBinCenterYFromIbinY(ibiny);
                double bin_width_x =
                    hd2d_arr[iframe]->GetHi2d()->GetBinWidthX();
                double bin_width_y =
                    hd2d_arr[iframe]->GetHi2d()->GetBinWidthY();
                double bin_lo_x = bin_center_x - bin_width_x/2.0;
                double bin_up_x = bin_center_x + bin_width_x/2.0;
                double bin_lo_y = bin_center_y - bin_width_y/2.0;
                double bin_up_y = bin_center_y + bin_width_y/2.0;


                double day = obsmjd_arr[iframe] - obsmjd_arr[0];

                double velocity = 5.5e-6;
                double factor = 1./(1 + day * velocity);

                double bin_lo_x_new =
                    x_center + (bin_lo_x - x_center) * factor;
                double bin_up_x_new =
                    x_center + (bin_up_x - x_center) * factor;
                double bin_lo_y_new =
                    y_center + (bin_lo_y - y_center) * factor;
                double bin_up_y_new =
                    y_center + (bin_up_y - y_center) * factor;
                
                double oval_new = oval * pow(factor, 2);

                Interval* interval_x = new Interval;
                interval_x->InitSet(bin_lo_x_new, bin_up_x_new);
                Interval* interval_y = new Interval;
                interval_y->InitSet(bin_lo_y_new, bin_up_y_new);

                long ibinx_lo_frac = hd2d_arr[iframe]->GetHi2d()->GetIbinXFromX(bin_lo_x_new);
                if(ibinx_lo_frac < 0){
                    ibinx_lo_frac = 0;
                }
                long ibinx_up_frac = hd2d_arr[iframe]->GetHi2d()->GetIbinXFromX(bin_up_x_new);
                if(hd2d_arr[iframe]->GetNbinX() <= ibinx_up_frac){
                    ibinx_up_frac = hd2d_arr[iframe]->GetNbinX() - 1;
                }
                long ibiny_lo_frac = hd2d_arr[iframe]->GetHi2d()->GetIbinYFromY(bin_lo_y_new);
                if(ibiny_lo_frac < 0){
                    ibiny_lo_frac = 0;
                }
                long ibiny_up_frac = hd2d_arr[iframe]->GetHi2d()->GetIbinYFromY(bin_up_y_new);
                if(hd2d_arr[iframe]->GetNbinY() <= ibiny_up_frac){
                    ibiny_up_frac = hd2d_arr[iframe]->GetNbinY() - 1;
                }
                HistDataNerr2d* hd2d_frac = hd2d_arr[iframe]->GenSubHist(
                    ibinx_lo_frac, ibinx_up_frac,
                    ibiny_lo_frac, ibiny_up_frac);
                hd2d_frac->SetFracAtIntervalXY(interval_x, interval_y);

                // fill
                for(long ibinx_sub = 0; ibinx_sub < hd2d_frac->GetNbinX(); ibinx_sub++){
                    for(long ibiny_sub = 0; ibiny_sub < hd2d_frac->GetNbinY(); ibiny_sub++){
                        double oval_new_mod = oval_new * hd2d_frac->GetOvalElm(ibinx_sub, ibiny_sub);
                        hd2d_exp_arr[iframe]->Fill(hd2d_frac->GetHi2d()->GetBinCenterXFromIbinX(ibinx_sub),
                                                   hd2d_frac->GetHi2d()->GetBinCenterYFromIbinY(ibiny_sub),
                                                   oval_new_mod);
                    }
                }
                delete interval_x;
                delete interval_y;
                delete hd2d_frac;
            }
        }
    }

    // expand
    double* cube_exp_mat = new double [npix_cube];

    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        for(long ipix_x = 0; ipix_x < img_info_in->GetNaxesArrElm(0); ipix_x ++){
            for(long ipix_y = 0; ipix_y < img_info_in->GetNaxesArrElm(1); ipix_y ++){
                long ipix = iframe * img_info_in->GetNpixelImg()
                    + ipix_y * img_info_in->GetNaxesArrElm(0)
                    + ipix_x;
                cube_exp_mat[ipix] = hd2d_exp_arr[iframe]->GetOvalElm(ipix_x, ipix_y);
            }
        }
    }

    // calc residual
    double** oval_arr = new double* [img_info_in->GetNframe()];
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        long npixel = img_info_in->GetNaxesArrElm(0) * img_info_in->GetNaxesArrElm(1);
        oval_arr[iframe] = new double [npixel];
        for(long ipixel = 0; ipixel < npixel; ipixel ++){
            oval_arr[iframe][ipixel] = hd2d_exp_arr[iframe]->GetOvalArr()->GetValElm(ipixel);
        }
    }


    DataArrayNerr1d** da1d_arr = new DataArrayNerr1d* [img_info_in->GetNframe()];
    for(long iframe = 0; iframe < img_info_in->GetNframe(); iframe ++){
        da1d_arr[iframe] = new DataArrayNerr1d;
        da1d_arr[iframe]->Copy(hd2d_exp_arr[iframe]->GetOvalArr());
    }
    DataArrayNerr1d* da1d_var = new DataArrayNerr1d;
    DataArray1dOpe::GetVariance(da1d_arr, img_info_in->GetNframe(),
                                da1d_var);
    printf("var = %e\n", MirMath::GetAdd(da1d_var->GetNdata(), da1d_var->GetVal()));


    float* cube_float_mat = new float [npix_cube];
    for(long ipix = 0; ipix < npix_cube; ipix ++){
        cube_float_mat[ipix] = cube_exp_mat[ipix];
    }
    float* cube_log_float_mat = new float [npix_cube];
    double pval_min = 1.0e10;
    double pval_max = -1.0e10;
    for(long ipix = 0; ipix < npix_cube; ipix ++){
        if(cube_exp_mat[ipix] < pval_min){
            pval_min = cube_exp_mat[ipix];
        }
        if(cube_exp_mat[ipix] > pval_max){
            pval_max = cube_exp_mat[ipix];
        }        
    }
    double pval_offset = pval_min - (pval_max - pval_min) * 0.1;
    for(long ipix = 0; ipix < npix_cube; ipix ++){
        cube_log_float_mat[ipix] = log(cube_exp_mat[ipix] - pval_offset);
    }
    
    MifImgInfo* img_info_out = new MifImgInfo;
    img_info_out->InitSetCube(1, 1, 1,
                              img_info_in->GetNaxesArrElm(0),
                              img_info_in->GetNaxesArrElm(1),
                              img_info_in->GetNaxesArrElm(2));

    MifFits::OutFitsCubeF(argval->GetOutdir(),
                         argval->GetOutfileHead(),
                         "cube",
                         3, img_info_out->GetNaxesArr(),
                         cube_float_mat);

    MifFits::OutFitsCubeF(argval->GetOutdir(),
                         argval->GetOutfileHead(),
                         "cube_log",
                         3, img_info_out->GetNaxesArr(),
                         cube_log_float_mat);
    
    return status_prog;
}



