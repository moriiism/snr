#include "fitsio.h"
#include "mi_str.h"
#include "mi_iolib.h"
#include "mi_fits.h"
#include "arg_img2cube.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValImg2cube* argval = new ArgValImg2cube;
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


    printf("--- imglist ---\n");
    
    string* imglist = NULL;
    long nline_imglist = 0;
    MiIolib::GenReadFileSkipComment(argval->GetInList(),
                                    &imglist, &nline_imglist);
    printf("nline_imglist = %ld\n", nline_imglist);
    for(long iline = 0; iline < nline_imglist; iline ++){
        printf("%s\n", imglist[iline].c_str());
    }
    printf("=== imglist ===\n");

    printf("--- img_info ---\n");
    MiImgInfo* img_info = new MiImgInfo;
    img_info->Load(argval->GetSubimgDat());
    img_info->PrintInfo();
    printf("=== img_info ===\n");
    

    long npix_cube =
        img_info->GetNaxesArrElm(0) *
        img_info->GetNaxesArrElm(1) *
        nline_imglist;
    double* cube_mat = new double [npix_cube];
    
    long index = 0;
    for(long iline = 0; iline < nline_imglist; iline ++){
        double* X_mat = NULL;
        MiFits::InFitsImageF(imglist[iline],
                             img_info, &X_mat);
        for(long ipix = 0; ipix < img_info->GetNpixelImg(); ipix ++){
            cube_mat[index] = X_mat[ipix];
            index ++;
        }
        delete [] X_mat;
    }

    

    float* cube_float_mat = new float [npix_cube];
    for(long ipix = 0; ipix < npix_cube; ipix ++){
        cube_float_mat[ipix] = cube_mat[ipix];
    }
    float* cube_log_float_mat = new float [npix_cube];
    double pval_min = 1.0e10;
    double pval_max = -1.0e10;
    for(long ipix = 0; ipix < npix_cube; ipix ++){
        if(cube_mat[ipix] < pval_min){
            pval_min = cube_mat[ipix];
        }
        if(cube_mat[ipix] > pval_max){
            pval_max = cube_mat[ipix];
        }        
    }
    double pval_offset = pval_min - (pval_max - pval_min) * 0.1;
    for(long ipix = 0; ipix < npix_cube; ipix ++){
        cube_log_float_mat[ipix] = log(cube_mat[ipix] - pval_offset);
    }
    
    MiImgInfo* img_info_cube = new MiImgInfo;
    img_info_cube->InitSetCube(1, 1, 1,
                               img_info->GetNaxesArrElm(0),
                               img_info->GetNaxesArrElm(1),
                               nline_imglist);

    MiFits::OutFitsCubeF(argval->GetOutdir(),
                         argval->GetOutfileHead(),
                         "cube",
                         3, img_info_cube->GetNaxesArr(),
                         cube_float_mat);

    MiFits::OutFitsCubeF(argval->GetOutdir(),
                         argval->GetOutfileHead(),
                         "cube_log",
                         3, img_info_cube->GetNaxesArr(),
                         cube_log_float_mat);
    
    
    
    return status_prog;
}
