#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <direct.h>

#define mytype double

typedef struct _lab_info
{
    char ph[32];
    char word[32];
    mytype begin;
    mytype end;
} labinfo;

labinfo a[10000];

typedef struct _mfc_info
{
    char mfc[1024];
} mfcinfo;

mfcinfo b[50000];

typedef struct _finalout
{
    char word[32];
    mytype begin;
    mytype end;
    char mfc;
} finalout;

finalout c[10000];

int main(int argc,char *argv[])
{
    //if(argc<2)
        //fprintf(stderr,"open fail!");
    FILE *pfile,*qfile;
    FILE *out3;
    char line[1024],line2[1024];
    char *token;
    int i=0,cnt=0,cnt2=0,j=0;
    int mfccbegin=0,mfccend=0;

    for(i=0; i<10000; i++)
    {
        memset(a[i].ph,0,32);
        memset(a[i].word,0,32);
    }

    pfile = fopen(argv[1],"r");
    qfile = fopen(argv[2],"r");
    if( !pfile )
        fprintf(stderr, "Cannot open lab.txt\n");
    if( !qfile )
        fprintf(stderr, "Cannot open mfcc.txt\n");

    out3 = fopen(".\\DATA\\ExeSource\\finallabout.txt","w");
    if(!out3) fprintf(stderr,"fail\n");
    i = 0;
    fgets(line, 1023, pfile);
    fgets(line, 1023, pfile);
    while( !feof(pfile)  )
    {
        fgets(line, 1023, pfile);
        //printf("%d\n", i);
        token=strtok(line," \n");
        if( token )
        {
            if( strcmp(token, ".")==0 )
                break;
            else
            {
                a[i].begin=atof(token);
                token = strtok(NULL," \n");
                if ( token )
                    a[i].end = atof(token);
                else
                {
                    // format error
                    break;
                }

                token = strtok(NULL," \n");
                if ( token )
                    strcpy(a[i].ph,token);
                else
                {
                    // format error
                    break;
                }
                token = strtok(NULL," \n");
                if ( token )
                {
                    strcpy(a[i].word, token);
                }
                else
                {
                    // memset( a[i].word, 0, sizeof(char)*32 );
                    strcpy(a[i].word, "0");
                }

            }
            i ++;
        }
    }

    cnt = i;
    i=0;

    /*for(i=0; i<cnt; i++)
    {
        fprintf(out,"%s",a[i].word);
        fprintf(out,"\n");
        fprintf(out,"%s",a[i].ph);
        fprintf(out,"\n");
        fprintf(out,"%lf\n",a[i].begin);
        fprintf(out,"%lf\n",a[i].end);
    }
    fclose(out);*/
    i=0;

    while( !feof(qfile) )
    {
        fgets(line2, 1023, qfile);
        strcpy(b[i].mfc, line2);
        i++;
    }

    cnt2=i;


    i=0;
    int ii=0;

    for(i=0; i<cnt; i++)
    {
        if( strcmp(a[i].word,"0")!=0)
        {
            strcpy( c[ii].word, a[i].word);
            c[ii].begin=a[i].begin;
            //printf("1");
        }
        if( strcmp(a[i].ph,"sp")==0 || strcmp(a[i].ph,"sil")==0)
        {
            c[ii].end=a[i].begin;
            ii++;
            //printf("2");
        }

    }
    for(i=1; i<ii-1; i++)
    {
        fprintf(out3,"%s\n",c[i].word);
        fprintf(out3,"%.2lf\n",c[i].begin*0.0000001);
        fprintf(out3,"%.2lf\n",c[i].end*0.0000001);
        mfccbegin=c[i].begin*0.0001;
        mfccend=c[i].end*0.0001;
        /*for(j=mfccbegin;j<=mfccend;j+=10)
        {
         fprintf(out4,"%s\n",b[j/10-1].mfc);
        }*/
    }
    fclose(out3);

    FILE *fout;
    FILE *title;
    char *token1;
    char titlename[1024];
    title = fopen(".\\DATA\\Listfile\\ref.txt","r");
    fgets(titlename, 1023, title);
    printf("%s\n", titlename);
    token1 = strtok( titlename, "\n");
    printf("%s\n", token1);
    char filename[1024];

    //if(listout == NULL) fprintf(stderr,"ERROR!\n");
    for(i=1; i<ii-1; i++)
    {
     sprintf(filename,".\\DATA\\Listfile\\%s\\%d_%s.txt", token1, i, c[i].word);
     fout = fopen( filename, "w");
     mfccbegin=c[i].begin*0.0001;
     mfccend=c[i].end*0.0001;
     for(j=mfccbegin;j<=mfccend;j+=10)
        {
         fprintf(fout,"%s",b[j/10-1].mfc);
        }
     fclose(fout);
    }




    return 0;

}
