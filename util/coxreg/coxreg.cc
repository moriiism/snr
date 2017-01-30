#include "mi_str.h"
#include "mi_iolib.h"
#include "mir_hist2d_nerr.h"
#include "mir_qdp_tool.h"
#include "mir_graph2d_ope.h"
#include "mir_root_tool.h"
#include "arg_coxreg.h"
#include "TCanvas.h"
#include "TH1D.h"


// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;

    MirRootTool* root_tool = new MirRootTool;
    root_tool->InitTCanvas("def");
    
    ArgValCoxreg* argval = new ArgValCoxreg;
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

    long nline = 0;
    string* lines_arr = NULL;
    MiIolib::GenReadFileSkipComment(argval->GetInFile(),
                           &lines_arr, &nline);
    int nsplit = 0; 
    string* split_arr = NULL;
    for(long iline = 0; iline < nline; iline ++){
        if( "polygon" == lines_arr[iline].substr(0, 7)){
            MiStr::GenSplit(lines_arr[iline].
                            substr(8, lines_arr[iline].length() - 9),
                            &nsplit, &split_arr, ",");
        }
    }
    MiIolib::DelReadFile(lines_arr);
    
    int npoint = nsplit / 2;
    double* xpoint_arr = new double [npoint + 1];
    double* ypoint_arr = new double [npoint + 1];
    for(int ipoint = 0; ipoint < npoint; ipoint ++){
        xpoint_arr[ipoint] = atof(split_arr[ipoint * 2].c_str());
        ypoint_arr[ipoint] = atof(split_arr[ipoint * 2 + 1].c_str());
    }
    xpoint_arr[npoint] = atof(split_arr[0].c_str());
    ypoint_arr[npoint] = atof(split_arr[1].c_str());
    MiStr::DelSplit(split_arr);
    
    GraphDataNerr2d* gd2d = new GraphDataNerr2d;
    gd2d->Init(npoint + 1);
    gd2d->SetXvalArr(npoint + 1, xpoint_arr);
    gd2d->SetOvalArr(npoint + 1, ypoint_arr);

    // -------------------------
    long npix = 512;
    HistDataNerr2d* hd2d_md = new HistDataNerr2d;
    hd2d_md->Init(npix, 0.0, npix, npix, 0.0, npix);
    vector<double> mdx_vec;
    vector<double> mdy_vec;
    HistData2dOpe::FillGd2d(gd2d, hd2d_md);

    char outfile[kLineSize];
    sprintf(outfile, "%s/midpoint.dat", argval->GetOutdir().c_str());
    FILE* fp = fopen(outfile, "w");
    
    int ntheta = argval->GetNtheta();
    double theta_st = 0.0;
    double theta_ed = M_PI;
    double delta_theta = (theta_ed - theta_st) / ntheta;
    for(int itheta = 0; itheta < ntheta; itheta ++){
        double theta = theta_st + delta_theta * itheta;
        // motion
        GraphDataNerr2d* gd2d_conv = new GraphDataNerr2d;
        GraphData2dOpe::GetMotion(gd2d, 0.0, 0.0, theta, 1, gd2d_conv);
        
        double center_x = (gd2d_conv->GetXvalArr()->GetValMin() +
                           gd2d_conv->GetXvalArr()->GetValMax()) / 2.0;
        double center_y = (gd2d_conv->GetOvalArr()->GetValMin() +
                           gd2d_conv->GetOvalArr()->GetValMax()) / 2.0;
        double lo_x = center_x - npix/2.;
        double up_x = center_x + npix/2.;
        double lo_y = center_y - npix/2.;
        double up_y = center_y + npix/2.;

        HistDataNerr2d* hd2d_fill = new HistDataNerr2d;
        hd2d_fill->Init(npix, lo_x, up_x, npix, lo_y, up_y);
        HistData2dOpe::FillGd2d(gd2d_conv, hd2d_fill);

        char outfig[kLineSize];
        sprintf(outfig, "%s/%s_%4.4d.png",
                argval->GetOutdir().c_str(),
                argval->GetOutfileHead().c_str(),
                itheta);
        hd2d_fill->MkTH2Fig(outfig, root_tool);
        
        double xval_md = HistData2dOpe::FindMdXbyEdge(hd2d_fill);
        double yval_md = HistData2dOpe::FindMdYbyEdge(hd2d_fill);
        Vect2d* vect_md = new Vect2d;
        vect_md->Init(xval_md, yval_md);
        Vect2d* vect_md_org = MirGeom::GenRot(vect_md, theta, -1);

        printf("%e %e\n", vect_md_org->GetPosX(), vect_md_org->GetPosY());
        fprintf(fp, "%e %e\n", vect_md_org->GetPosX(), vect_md_org->GetPosY());

        mdx_vec.push_back(vect_md_org->GetPosX());
        mdy_vec.push_back(vect_md_org->GetPosY());
        hd2d_md->Fill(vect_md_org->GetPosX(), vect_md_org->GetPosY());

        delete gd2d_conv;
        delete hd2d_fill;
        delete vect_md;
        delete vect_md_org;
    }
    fclose(fp);
    
    double cox_x = MirMath::GetAMean(mdx_vec);
    double cox_y = MirMath::GetAMean(mdy_vec);
    double cox_x_stddev = MirMath::GetStddev(mdx_vec);
    double cox_y_stddev = MirMath::GetStddev(mdy_vec);
    printf("cox = (%e, %e)\n", cox_x, cox_y);
    printf("cox_stddev = (%e, %e)\n", cox_x_stddev, cox_y_stddev);

    char summary_file[kLineSize];
    sprintf(summary_file, "%s/summary.dat", argval->GetOutdir().c_str());
    FILE* fp_sum = fopen(summary_file, "w");
    fprintf(fp_sum, "cox = (%e, %e)\n", cox_x, cox_y);
    fprintf(fp_sum, "cox_stddev = (%e, %e)\n", cox_x_stddev, cox_y_stddev);
    fclose(fp_sum);

    TH2D* td2d = hd2d_md->GenTH2D(0.0, 0.0, 0.0);
    TCanvas* can = new TCanvas;
    td2d->Draw("colz");
    char outfig[kLineSize];
    sprintf(outfig, "%s/center.png", argval->GetOutdir().c_str());
    can->Print(outfig);
    
    delete gd2d;
    delete td2d;
    delete can;
    
    return status_prog;
}
