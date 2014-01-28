#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "sys/time.h"

#define PIE 3.1415926

#define POSITION_INDEX(_z,_y,_x) (_z*ny*nx + _y*nx + _x)

typedef struct _POSITION_DATA{
    float u  ;
    float v  ;
    float w  ;
    float up ;
    float up1; 
    float up2; 
    float vp ;
    float vp1;
    float vp2; 
    float wp ; 
    float wp1; 
    float wp2; 
    float us ;
    float us1; 
    float us2; 
    float vs ; 
    float vs1; 
    float vs2; 
    float ws ; 
    float ws1; 
    float ws2; 
}POSITION_DATA,*PPOSITION_DATA;

int main(int argc, char **argv)
{
    int i,j,k,kk,kkk,l,mm=5;
    int nx,ny,nz,lt,nedge;
    int nleft,nright,nfront,nback,ntop,nbottom;
    float frequency;
    float velmax;
    float dt;
    int ncx_shot1,ncy_shot1,ncz_shot;
    int ishot,ncy_shot,ncx_shot;
    float unit;
    int nxshot,nyshot,dxshot,dyshot;
    char infile[80],outfile[80],logfile[80],tmp[80];
    FILE  *fin, *fout, *flog;
    struct timeval start,end;
    float all_time;

    float *vpp, *density, *vss;
    float c[5][7];
    float *wave;
    float nshot,t0,tt,c0;
    float dtx,dtz,dtxz,dr1,dr2,dtx4,dtz4,dtxz4;
    float xmax,px,sx;
    float vvp2,drd1,drd2,vvs2,tempux2,tempuy2,tempuz2,tempvx2,tempvy2,tempvz2,
          tempwx2,tempwy2,tempwz2,tempuxz,tempuxy,tempvyz,tempvxy,tempwxz,tempwyz;
    if(argc<4)
    {
        printf("please add 3 parameter: inpurfile, outfile, logfile\n");
        exit(0);
    }

    strcpy(infile,argv[1]);
    strcpy(outfile,argv[2]);
    strcpy(logfile,argv[3]);

    strcpy(tmp,"date ");
    strncat(tmp, ">> ",3);
    strncat(tmp, logfile, strlen(logfile));
    flog = fopen(logfile,"w");
    fprintf(flog,"------------start time------------\n");
    fclose(flog);
    system(tmp);
    gettimeofday(&start,NULL);

    fin = fopen(infile,"r");
    if(fin == NULL)
    {
        printf("file %s is  not exist\n",infile);
        exit(0);
    }
    fscanf(fin,"nx=%d\n",&nx);
    fscanf(fin,"ny=%d\n",&ny);
    fscanf(fin,"nz=%d\n",&nz);
    fscanf(fin,"lt=%d\n",&lt);
    fscanf(fin,"nedge=%d\n",&nedge);
    fscanf(fin,"ncx_shot1=%d\n",&ncx_shot1);
    fscanf(fin,"ncy_shot1=%d\n",&ncy_shot1);
    fscanf(fin,"ncz_shot=%d\n",&ncz_shot);
    fscanf(fin,"nxshot=%d\n",&nxshot);
    fscanf(fin,"nyshot=%d\n",&nyshot);
    fscanf(fin,"frequency=%f\n",&frequency);
    fscanf(fin,"velmax=%f\n",&velmax);
    fscanf(fin,"dt=%f\n",&dt);
    fscanf(fin,"unit=%f\n",&unit);
    fscanf(fin,"dxshot=%d\n",&dxshot);
    fscanf(fin,"dyshot=%d\n",&dyshot);
    fclose(fin);

    printf("\n--------workload parameter--------\n");
    printf("nx=%d\n",nx);
    printf("ny=%d\n",ny);
    printf("nz=%d\n",nz);
    printf("lt=%d\n",lt);
    printf("nedge=%d\n",nedge);
    printf("ncx_shot1=%d\n",ncx_shot1);
    printf("ncy_shot1=%d\n",ncy_shot1);
    printf("ncz_shot=%d\n",ncz_shot);
    printf("nxshot=%d\n",nxshot);
    printf("nyshot=%d\n",nyshot);
    printf("frequency=%f\n",frequency);
    printf("velmax=%f\n",velmax);
    printf("dt=%f\n",dt);
    printf("unit=%f\n",unit);
    printf("dxshot=%d\n",dxshot);
    printf("dyshot=%d\n\n",dyshot);
    flog = fopen(logfile,"a");
    fprintf(flog,"\n--------workload parameter--------\n");
    fprintf(flog,"nx=%d\n",nx);
    fprintf(flog,"ny=%d\n",ny);
    fprintf(flog,"nz=%d\n",nz);
    fprintf(flog,"lt=%d\n",lt);
    fprintf(flog,"nedge=%d\n",nedge);
    fprintf(flog,"ncx_shot1=%d\n",ncx_shot1);
    fprintf(flog,"ncy_shot1=%d\n",ncy_shot1);
    fprintf(flog,"ncz_shot=%d\n",ncz_shot);
    fprintf(flog,"nxshot=%d\n",nxshot);
    fprintf(flog,"nyshot=%d\n",nyshot);
    fprintf(flog,"frequency=%f\n",frequency);
    fprintf(flog,"velmax=%f\n",velmax);
    fprintf(flog,"dt=%f\n",dt);
    fprintf(flog,"unit=%f\n",unit);
    fprintf(flog,"dxshot=%d\n",dxshot);
    fprintf(flog,"dyshot=%d\n\n",dyshot);
    fclose(flog);

    const int nSize = nz*ny*nx；

    PPOSITION_DATA pPositionData = (PPOSITION_DATA)malloc(sizeof(POSITION_DATA)*(nSize));

    vpp     = (float*)malloc(sizeof(float)*nSize);
    density = (float*)malloc(sizeof(float)*nSize);
    vss     = (float*)malloc(sizeof(float)*nSize);
    wave = (float*)malloc(sizeof(float)*lt);

    nshot=nxshot*nyshot;
    t0=1.0/frequency;
    for(i=0;i<nz;i++)
        for(j=0;j<ny;j++)
            for(k=0;k<nx;k++)
            {
                int nIndex = POSITION_INDEX(i,j,k);
                if(i<210)
                {
                    vpp[nIndex]=2300.;
                    vss[nIndex]=1232.;
                    density[nIndex]=1.;
                }
                else if(i>=210 && i<260)
                {
                    vpp[nIndex]=2800.;
                    vss[nIndex]=1509.;
                    density[nIndex]=2.;
                }
                else
                {
                    vpp[nIndex]=3500.;
                    vss[nIndex]=1909.;
                    density[nIndex]=2.5;
                }
            }

    for(l=0;l<lt;l++)
    {
        tt=l*dt;
        tt=tt-t0;
        float sp=PIE*frequency*tt;
        float fx=100000.*exp(-sp*sp)*(1.-2.*sp*sp);
        wave[l]=fx;
    }

    if(mm==5)
    {
        c0=-2.927222164;
        c[0][0]=1.66666665;
        c[1][0]=-0.23809525;
        c[2][0]=0.03968254;
        c[3][0]=-0.004960318;
        c[4][0]=0.0003174603;
    }

    c[0][1]=0.83333;
    c[1][1]=-0.2381;
    c[2][1]=0.0595;
    c[3][1]=-0.0099;
    c[4][1]=0.0008;

    for(i=0;i<5;i++)
        for(j=0;j<5;j++)
            c[j][2+i]=c[i][1]*c[j][1];

    dtx=dt/unit;
    dtz=dt/unit;
    dtxz=dtx*dtz;

    dr1=dtx*dtx/2.;
    dr2=dtz*dtz/2.;

    dtx4=dtx*dtx*dtx*dtx;
    dtz4=dtz*dtz*dtz*dtz;
    dtxz4=dtx*dtx*dtz*dtz;

    fout=fopen(outfile,"wb");
    for(ishot=1;ishot<=nshot;ishot++)
    {
        printf("shot=%d\n",ishot);
	flog = fopen(logfile,"a");
        fprintf(flog,"shot=%d\n",ishot);
	fclose(flog);
        ncy_shot=ncy_shot1+(ishot/nxshot)*dyshot;
        ncx_shot=ncx_shot1+(ishot%nxshot)*dxshot;

        memset(pPositionData,0,nSize*sizeof(POSITION_DATA));

        for(l=1;l<=lt;l++)
        {
            xmax=l*dt*velmax;
            nleft=ncx_shot-xmax/unit-10;
            nright=ncx_shot+xmax/unit+10;
            nfront=ncy_shot-xmax/unit-10;
            nback=ncy_shot+xmax/unit+10;
            ntop=ncz_shot-xmax/unit-10;
            nbottom=ncz_shot+xmax/unit+10;
            if(nleft<5) nleft=5;
            if(nright>nx-5) nright=nx-5;
            if(nfront<5) nfront=5;
            if(nback>ny-5) nback=ny-5;
            if(ntop<5) ntop=5;
            if(nbottom>nz-5) nbottom=nz-5;

            ntop = ntop-1;
            nfront = nfront-1;
            nleft = nleft-1;
            for(k=ntop;k<nbottom;k++)
                for(j=nfront;j<nback;j++)
                    for(i=nleft;i<nright;i++)
                    {
                        int nIndex              = POSITION_INDEX(k,j,i);
                        PPOSITION_DATA  pCurPos = pPositionData + nIndex;


                        if(i==ncx_shot-1&&j==ncy_shot-1&&k==ncz_shot-1)
                        {
                            px=1.;
                            sx=0.;
                        }
                        else
                        {
                            px=0.;
                            sx=0.;
                        }
                        vvp2=vpp[nIndex]*vpp[nIndex];
                        drd1=dr1*vvp2;
                        drd2=dr2*vvp2;

                        vvs2=vss[nIndex]*vss[nIndex];
                        drd1=dr1*vvs2;
                        drd2=dr2*vvs2;

                        tempux2=0.0f;
                        tempuy2=0.0f;
                        tempuz2=0.0f;
                        tempvx2=0.0f;
                        tempvy2=0.0f;
                        tempvz2=0.0f;
                        tempwx2=0.0f;
                        tempwy2=0.0f;
                        tempwz2=0.0f;
                        tempuxz=0.0f;
                        tempuxy=0.0f;
                        tempvyz=0.0f;
                        tempvxy=0.0f;
                        tempwxz=0.0f;
                        tempwyz=0.0f;
                        for(kk=1;kk<=mm;kk++)
                        {
                            tempux2=tempux2+c[kk-1][0]*(u[k*ny*nx+j*nx+(i+kk)]+u[k*ny*nx+j*nx+(i-kk)]);
                            tempuy2=tempuy2+c[kk-1][0]*(u[k*ny*nx+(j+kk)*nx+i]+u[k*ny*nx+(j-kk)*nx+i]);
                            tempuz2=tempuz2+c[kk-1][0]*(u[(k+kk)*ny*nx+j*nx+i]+u[(k-kk)*ny*nx+j*nx+i]);

                            tempvx2=tempvx2+c[kk-1][0]*(v[k*ny*nx+j*nx+(i+kk)]+v[k*ny*nx+j*nx+(i-kk)]);
                            tempvy2=tempvy2+c[kk-1][0]*(v[k*ny*nx+(j+kk)*nx+i]+v[k*ny*nx+(j-kk)*nx+i]);
                            tempvz2=tempvz2+c[kk-1][0]*(v[(k+kk)*ny*nx+j*nx+i]+v[(k-kk)*ny*nx+j*nx+i]);

                            tempwx2=tempwx2+c[kk-1][0]*(w[k*ny*nx+j*nx+(i+kk)]+w[k*ny*nx+j*nx+(i-kk)]);
                            tempwy2=tempwy2+c[kk-1][0]*(w[k*ny*nx+(j+kk)*nx+i]+w[k*ny*nx+(j-kk)*nx+i]);
                            tempwz2=tempwz2+c[kk-1][0]*(w[(k+kk)*ny*nx+j*nx+i]+w[(k-kk)*ny*nx+j*nx+i]);

                        } //for(kk=1;kk<=mm;kk++) end

                        tempux2=(tempux2+c0*u[nIndex])*vvp2*dtx*dtx;
                        tempuy2=(tempuy2+c0*u[nIndex])*vvs2*dtx*dtx;
                        tempuz2=(tempuz2+c0*u[nIndex])*vvs2*dtz*dtz;

                        tempvx2=(tempvx2+c0*v[nIndex])*vvs2*dtx*dtx;
                        tempvy2=(tempvy2+c0*v[nIndex])*vvp2*dtx*dtx;
                        tempvz2=(tempvz2+c0*v[nIndex])*vvs2*dtz*dtz;

                        tempwx2=(tempwx2+c0*w[nIndex])*vvs2*dtx*dtx;
                        tempwy2=(tempwy2+c0*w[nIndex])*vvs2*dtx*dtx;
                        tempwz2=(tempwz2+c0*w[nIndex])*vvp2*dtz*dtz;

                        for(kk=1;kk<=mm;kk++)
                        {
                            for(kkk=1;kkk<=mm;kkk++)
                            {
                                tempuxz=tempuxz+c[kkk-1][1+kk]*(u[(k+kkk)*ny*nx+j*nx+(i+kk)]
                                                   -u[(k-kkk)*ny*nx+j*nx+(i+kk)]
                                                   +u[(k-kkk)*ny*nx+j*nx+(i-kk)]
                                                   -u[(k+kkk)*ny*nx+j*nx+(i-kk)]);
                                tempuxy=tempuxy+c[kkk-1][1+kk]*(u[k*ny*nx+(j+kkk)*nx+(i+kk)]
                                                   -u[k*ny*nx+(j-kkk)*nx+(i+kk)]
                                                   +u[k*ny*nx+(j-kkk)*nx+(i-kk)]
                                                   -u[k*ny*nx+(j+kkk)*nx+(i-kk)]);

                                tempvyz=tempvyz+c[kkk-1][1+kk]*(v[(k+kkk)*ny*nx+(j+kk)*nx+i]
                                                   -v[(k-kkk)*ny*nx+(j+kk)*nx+i]
                                                   +v[(k-kkk)*ny*nx+(j-kk)*nx+i]
                                                   -v[(k+kkk)*ny*nx+(j-kk)*nx+i]);
                                tempvxy=tempvxy+c[kkk-1][1+kk]*(v[k*ny*nx+(j+kkk)*nx+(i+kk)]
                                                   -v[k*ny*nx+(j-kkk)*nx+(i+kk)]
                                                   +v[k*ny*nx+(j-kkk)*nx+(i-kk)]
                                                   -v[k*ny*nx+(j+kkk)*nx+(i-kk)]);

                                tempwyz=tempwyz+c[kkk-1][1+kk]*(w[(k+kkk)*ny*nx+(j+kk)*nx+i]
                                                   -w[(k-kkk)*ny*nx+(j+kk)*nx+i]
                                                   +w[(k-kkk)*ny*nx+(j-kk)*nx+i]
                                                   -w[(k+kkk)*ny*nx+(j-kk)*nx+i]);
                                tempwxz=tempwxz+c[kkk-1][1+kk]*(w[(k+kkk)*ny*nx+j*nx+(i+kk)]
                                                   -w[(k-kkk)*ny*nx+j*nx+(i+kk)]
                                                   +w[(k-kkk)*ny*nx+j*nx+(i-kk)]
                                                   -w[(k+kkk)*ny*nx+j*nx+(i-kk)]);
                            } // for(kkk=1;kkk<=mm;kkk++) end
                        } //for(kk=1;kk<=mm;kk++) end
                        pCurPos->up=2.*pCurPos->up1-pCurPos->up2
                                          +tempux2+tempwxz*vvp2*dtz*dtx
                                          +tempvxy*vvp2*dtz*dtx;

                        pCurPos->vp=2.*pCurPos->vp1-pCurPos->vp2
                                          +tempvy2+tempuxy*vvp2*dtz*dtx
                                          +tempwyz*vvp2*dtz*dtx;
                        
                        pCurPos->wp=2.*pCurPos->wp1-pCurPos->wp2
                                          +tempwz2+tempuxz*vvp2*dtz*dtx
                                          +tempvyz*vvp2*dtz*dtx
                                          +px*wave[l-1];
                        
                        pCurPos->us=2.*pCurPos->us1-pCurPos->us2
                                          +tempuy2+tempuz2
                                          -tempvxy*vvs2*dtz*dtx-tempwxz*vvs2*dtz*dtx;
                        
                        pCurPos->vs=2.*pCurPos->vs1-pCurPos->vs2
                                          +tempvx2+tempvz2
                                          -tempuxy*vvs2*dtz*dtx-tempwyz*vvs2*dtz*dtx;
                        
                        pCurPos->ws=2.*pCurPos->ws1-pCurPos->ws2
                                          +tempwx2+tempwy2
                                          -tempuxz*vvs2*dtz*dtx-tempvyz*vvs2*dtz*dtx;
                    }//for(i=nleft;i<nright;i++) end

            for(k=ntop;k<nbottom;k++)
                for(j=nfront;j<nback;j++)
                    for(i=nleft;i<nright;i++)
                    {

                        int nIndex              = POSITION_INDEX(k,j,i);
                        PPOSITION_DATA  pCurPos = pPositionData + nIndex;

                        pCurPos->u = pCurPos->up + pCurPos->us;
                        pCurPos->v = pCurPos->vp + pCurPos->vs;
                        pCurPos->w = pCurPos->wp + pCurPos->ws;

                        pCurPos->up2 = pCurPos->up1;
                        pCurPos->up1 = pCurPos->up;

                        pCurPos->vp2 = pCurPos->vp1;
                        pCurPos->vp1 = pCurPos->vp;

                        pCurPos->wp2 = pCurPos->wp1;
                        pCurPos->wp1 = pCurPos->wp;

                        pCurPos->ws2 = pCurPos->ws1;
                        pCurPos->ws1 = pCurPos->ws;

                    }//for(i=nleft;i<nright;i++) end
        }//for(l=1;l<=lt;l++) end
        fwrite(up+169*ny*nx,sizeof(float),ny*nx,fout);
    }//for(ishot=1;ishot<=nshot;ishot++) end
    fclose(fout);

    free(pPositionData);

    free(vpp);
    free(density);
    free(vss);
    free(wave);

    gettimeofday(&end,NULL);
    all_time = (end.tv_sec-start.tv_sec)+(float)(end.tv_usec-start.tv_usec)/1000000.0;
    printf("run time:\t%f s\n",all_time);
    flog = fopen(logfile,"a");
    fprintf(flog,"\nrun time:\t%f s\n\n",all_time);
    fclose(flog);
    flog = fopen(logfile,"a");
    fprintf(flog,"------------end time------------\n");
    fclose(flog);
    system(tmp);
    return 1;
}
