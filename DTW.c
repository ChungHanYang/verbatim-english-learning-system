#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//float mean_v[13] = {-14.10465867, -3.956956474,	-0.790181126,	-11.07623035,	-3.571850747,	-10.80528881,	-1.808322497,	-10.26091275,	-2.973750093,	-8.154729461,	-2.752483423,	-4.510126274,	110.7782739};

typedef struct node
{
    int x;
    int y;
    float value;
    struct node *bPt;
    struct node *fPt;
} Node;
typedef Node *NodePt;

typedef struct map
{
    double cost;
} mapping;
mapping asd[1024][1024];

typedef struct optresult
{
    int M;
    char ref[1024];
    char test[1024];
    char output[1024];
} Optpack;
typedef Optpack *OptionPt;

float dis(float *a, float *b, int Len)
{
    int i;
    double sum = 0.0;
    for( i = 0; i < Len; i++ )
    {
        sum += (a[i] > b[i] ? a[i] - b[i] : b[i] - a[i])*(a[i] > b[i] ? a[i] - b[i] : b[i] - a[i]);
    }
    return sqrt(sum);
}

Node *MinNode(Node *a, Node *b, Node *c)
{
    Node *min = a;
    min = min->value <= b->value ? min : b;
    return min = min->value <= c->value ? min : c;
}

void MetInitial(Node **DTW, int asize, int bsize)
{
    int i, j;
    for (i = 0; i <= asize; i++)
    {
        for (j = 0; j <= bsize; j++)
        {
            if( i == 0 || j == 0 )
                DTW[i][j].value = 1.0/0.0;
            DTW[i][j].x = i;
            DTW[i][j].y = j;
        }
    }
    DTW[0][0].value = 0;
}

void MetrixLink(Node **DTW, float *a, int asize, float *b, int bsize)
{
    Node *temp = &DTW[asize][bsize];
    while( temp->x != 0 && temp->y != 0 )
    {
        //printf("%d matches %d\n", temp->x, temp->y);
        temp->bPt->fPt = temp;
        temp = temp->bPt;
    }
}

Node **DTWDistance(float **a, int *asize, float **b, int *bsize)
{
    int i, j, k;
    float cost;
    Node **DTW, *dtw, *min;
    FILE *readaverage;
    char temp[1024],*token;
    double avr[13];
    readaverage = fopen(".\\DATA\\ExeSource\\average.txt","r");
    if( readaverage == NULL )fprintf( stderr, "open average fail!");

    // printf("\nqwe\n");
    while( !feof(readaverage) )
    {
     fgets( temp, 1023, readaverage);
     token = strtok( temp, " ");
     avr[0] = atof( token );
     //printf("%f\n",avr[0]);
     token = strtok( NULL, " ");
     avr[1] = atof( token );
     token = strtok( NULL, " ");
     avr[2] = atof( token );
     token = strtok( NULL, " ");
     avr[3] = atof( token );
     token = strtok( NULL, " ");
     avr[4] = atof( token );
     token = strtok( NULL, " ");
     avr[5] = atof( token );
     token = strtok( NULL, " ");
     avr[6] = atof( token );
     token = strtok( NULL, " ");
     avr[7] = atof( token );
     token = strtok( NULL, " ");
     avr[8] = atof( token );
     token = strtok( NULL, " ");
     avr[9] = atof( token );
     token = strtok( NULL, " ");
     avr[10] = atof( token );
     token = strtok( NULL, " ");
     avr[11] = atof( token );
     token = strtok( NULL, " ");
     avr[12] = atof( token );
     token = strtok( NULL, " ");
    }
     //for(i=0;i<13;i++)
        //printf("%f\n",avr[i]);
    DTW = (Node **)malloc((asize[0]+1)*sizeof(Node *)+(asize[0]+1)*(bsize[0]+1)*sizeof(Node));
    for (i = 0, dtw = (Node *)(DTW+asize[0]+1); i <= asize[0]; i++, dtw += (bsize[0]+1))
        DTW[i] = dtw;

    MetInitial(DTW, asize[0], bsize[0]);

    for (i = 1; i <= asize[0]; i++)
    {
        for (j = 1; j <= bsize[0]; j++)
        {
            for(k=0;k<13;k++)
                b[j-1][k] -= avr[k];

            cost = dis(a[i-1], b[j-1], asize[1]);			//easy to make mistake
            min = MinNode( &DTW[i-1][j-1], &DTW[i][j-1], &DTW[i-1][j] );
            DTW[i][j].value = cost + min->value;
            DTW[i][j].bPt = min;
            min->fPt = &DTW[i][j];

        }
    }
    MetrixLink(DTW, *a, asize[0], *b, bsize[0]);
    return DTW;
}

void FilePrepro(char *record, int *size)
{
    FILE *input;
    int element = 0, line = 0;
    float test;
    if ( (input = fopen( record, "r") ) == NULL )
        printf("Input file could not be opened...\n");
    else
    {
        do
        {
            test = fgetc(input);
            if( test == '\n' )
                line++;
            else if( test == ' ' )
                element++;
        }
        while ( !feof(input) );
        size[0] = line;
        size[1] = element/line;

        fclose(input);
    }
    //printf("%s %d %d\n", record, size[0], size[1]);
}

float **Spacecreate(int line, int element)
{
    int i;
    float **Str;
    Str = (float **) calloc(line, sizeof(float *));
    for(i=0; i<line; i++)
    {
        Str[i] = (float *) calloc(element, sizeof(float));
    }
    return Str;
}

void Spacedestroy(float **Str, int n)
{
    int i;
    if( Str )
    {
        for(i=0; i<n; i++)
        {
            free(Str[i]);
        }
        free(Str);
    }
}

void Fill_element(char *record, float **Str, int line, int element)
{
    FILE *input;
    int i, j;
    if ( (input = fopen( record, "r") ) == NULL )
        printf("File could not be opened...\n");
    else
    {
        for( i = 0; i < line; i++ )
            for( j = 0; j < element; j++ )
                fscanf(input, "%f", &Str[i][j]);
        fclose(input);
    }
}

void Usage(void)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "DTW - dynamic time wrapping stand-alone executable\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "  usage:\n");
    fprintf(stderr, "       DTW [ options ] feature_file_ref feature_file_test\n");
    fprintf(stderr,
            "  options:                                        [  def][ min--max]\n");
    fprintf(stderr,
            "    -M  d   : dimension of feature vector                   [  N][A]\n");
    fprintf(stderr, "  feature_file_ref:\n");
    fprintf(stderr, "    file containing feature vector for reference\n");
    fprintf(stderr, "  feature_file_test:\n");
    fprintf(stderr, "    file containing feature vector for test\n");
    fprintf(stderr, "  note:\n");
    fprintf(stderr, "    none\n");
    fprintf(stderr, "\n");

    exit(0);
}

void OptionIni( OptionPt pack )
{
    pack->M = 0;
    memset(pack->output, 0, sizeof(pack->output));
    memset(pack->ref, 0, sizeof(pack->ref));
    memset(pack->test, 0, sizeof(pack->test));
}

void Option(int argc, char **argv_search, OptionPt pack)
{
    int num_interp = 1, i;
    char *temp;
    while (--argc && argv_search != NULL)
    {
        if (**++argv_search == '-')
        {
            switch (*(*argv_search + 1))
            {
            case 'M':
            {
                num_interp = atoi(*++argv_search);
                if (num_interp < 1)
                    num_interp = 1;
                pack->M = num_interp;
                --argc;
                break;
            }
            case 'o':
            {
                *++argv_search;
                strcpy(pack->output, *argv_search);
                //printf("%s\n", pack->output);
                --argc;
                break;
            }
            default:
                printf("Invalid option '-%c %s'.\n", *(*argv_search + 1), *argv_search + 3);
            }
        }
        else
        {
            strcpy(pack->ref, *argv_search);
            //printf("%s\n", pack->ref);
            *++argv_search;
            argc--;
            strcpy(pack->test, *argv_search);
            //printf("%s\n", pack->test);
        }
    }
}

void ReversePrint(FILE *output, Node *tail)  		//this function has some problems
{
    while( tail->bPt )
    {
        tail = tail->bPt;
        ReversePrint( output, tail );
    }
    fprintf( output, "%.2f\n", tail->value );
}

void WriteOut(Node **table, int row, int col, char *Output)
{
    FILE *output;

    int cnt=0,i=0,j=0,total=0;
    double sum=0;
    int dif;
    Node *temp = &table[1][1];

    if ( (output = fopen( Output, "w") ) == NULL )
        printf("File could not be opened...\n");
    else
    {
        fprintf( output, "Ref_Frame#= %d\nTest_Frame#= %d\n", row, col );

        while( temp->fPt )
        {
            //fprintf( output, "%3d %3d %8.2f\n", temp->x, temp->y,
                     //(temp->value-temp->bPt->value) );
            if(abs(row-col)<20)
            {
             dif=abs(row-col);
             temp = temp->fPt;
             sum+=temp->value-temp->bPt->value;
             total++;
            }
            else
            {
             dif=abs(row-col);
             temp = temp->fPt;
             sum=1000000;
             total++;
            }

        }
        //fprintf( output, "%3d %3d %8.2f\n", temp->x, temp->y,
                 //(temp->value-temp->bPt->value) );
        //fprintf( output, "%d\n", total);
        fprintf( output, "%.2f\n", sum/total+0.05*(dif*dif));
    }


    fclose(output);
}

int main( int argc, char *argv[] )
{
    float **Pa = NULL, **Pb = NULL;
    int Asize[2] = {0}, Bsize[2] = {0};		//index 0 for lines and 1 for each line allocation
    Node **data = NULL;						//must be NULL to avoid dump stack
    Optpack res;
    OptionPt respt = &res;
    OptionIni(respt);
    //printf("\nqwe\n");
    if ( argc < 2 )							//User type check
        Usage();

    Option(argc, argv, respt);
    if( res.ref != NULL )
        FilePrepro( res.ref, Asize );
    if( res.test != NULL )
        FilePrepro( res.test, Bsize );
    if( respt->M != Asize[1] || respt->M != Bsize[1] )
    {
        printf("Dimension incorrect...terminated\n");
        return -1;
    }

    Pa = Spacecreate( Asize[0], Asize[1] );
    Pb = Spacecreate( Bsize[0], Bsize[1] );
    Fill_element( res.ref, Pa, Asize[0], Asize[1] );
    Fill_element( res.test, Pb, Bsize[0], Bsize[1] );
    data = DTWDistance(Pa, Asize, Pb, Bsize);
    if( strlen(res.output)>0 )

        WriteOut(data, Asize[0], Bsize[0], res.output);

    Spacedestroy(Pa, Asize[0]);
    Spacedestroy(Pb, Bsize[0]);
    if (data != NULL)
    {
        printf("DTW table was deleted...\n");
        free(data);
    }
    return 0;
}

//g++ -g DTW.c -o DTW -lm
