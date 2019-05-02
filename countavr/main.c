#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main()
{
 FILE *readtest;
 FILE *tyu;
 char line4[1024];
 int cnt2=0;
 double avr1=0,avr2=0,avr3=0,avr4=0,avr5=0,avr6=0,avr7=0,avr8=0,avr9=0,avr10=0,avr11=0,avr12=0,avr13=0;
 double total1=0,total2=0,total3=0,total4=0,total5=0,total6=0,total7=0,total8=0,total9=0,total10=0,total11=0,total12=0,total13=0;
 char *token;
 readtest = fopen(".\\DATA\\ExeSource\\uservoice.txt","r");
 tyu = fopen(".\\DATA\\ExeSource\\average.txt","w");
 if( readtest == NULL )fprintf( stderr, "open uservoice fail!");
 //count average
while( !feof( readtest ))
{
 fgets( line4, 1023, readtest);
 token = strtok( line4, " ");
 total1 += atof( token );
// printf("%d %f %f\n",cnt2+1, atof( token ), total1);
 token = strtok( NULL, " ");
 total2 += atof( token );
 //printf("%d %f %f\n",cnt2+1, atof( token ), total2);
 token = strtok( NULL, " ");
 total3 += atof( token );
 //printf("%d %f %f\n",cnt2+1, atof( token ), total3);
 token = strtok( NULL, " ");
 total4 += atof( token );
 token = strtok( NULL, " ");
 total5 += atof( token );
 token = strtok( NULL, " ");
 total6 += atof( token );
 token = strtok( NULL, " ");
 total7 += atof( token );
 token = strtok( NULL, " ");
 total8 += atof( token );
 token = strtok( NULL, " ");
 total9 += atof( token );
 token = strtok( NULL, " ");
 total10 += atof( token );
 token = strtok( NULL, " ");
 total11 += atof( token );
 token = strtok( NULL, " ");
 total12 += atof( token );
 token = strtok( NULL, " ");
 total13 += atof( token );
 token = strtok( NULL, " ");
 cnt2++;
}
//printf("%f\n",total1);
//printf("%f\n",total2);
//printf("%f\n",total3);
//printf("%f\n",total4);
//printf("%f\n",total5);
//printf("%d\n",cnt2-1);
 avr1 = total1/(cnt2-1);
 //printf("%f\n",avr1);
 avr2 = total2/(cnt2-1);
 //printf("%f\n",avr2);
 avr3 = total3/(cnt2-1);
 printf("%f\n",avr3);
 avr4 = total4/(cnt2-1);
 avr5 = total5/(cnt2-1);
 avr6 = total6/(cnt2-1);
 avr7 = total7/(cnt2-1);
 avr8 = total8/(cnt2-1);
 avr9 = total9/(cnt2-1);
 avr10 = total10/(cnt2-1);
 avr11 = total11/(cnt2-1);
 avr12 = total12/(cnt2-1);
 avr13 = total13/(cnt2-1);
 fprintf( tyu, "%f ", avr1);
 fprintf( tyu, "%f ", avr2);
 fprintf( tyu, "%f ", avr3);
 fprintf( tyu, "%f ", avr4);
 fprintf( tyu, "%f ", avr5);
 fprintf( tyu, "%f ", avr6);
 fprintf( tyu, "%f ", avr7);
 fprintf( tyu, "%f ", avr8);
 fprintf( tyu, "%f ", avr9);
 fprintf( tyu, "%f ", avr10);
 fprintf( tyu, "%f ", avr11);
 fprintf( tyu, "%f ", avr12);
 fprintf( tyu, "%f ", avr13);


}
