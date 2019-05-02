#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include "read_WAV_sec.h"

//#define INPUT "./wav/class.wav"
//#define OUTPUT "./wav/class.lab"
typedef struct EventTag{
	int s,t;
}EventTag;
typedef struct _Energy{
	double energy;
	double g_max,g_min;
	double g_max_smooth,g_min_smooth;
	double thr;
}Energy;


void globalEnergy(wave_t input,double **sampleFrame_fq,Energy *E,int index);
EventTag* endpointDetect(wave_t input,double **sampleCH_fq,Energy *E,int frame_num); //return an int array which record endpoint
Energy **malloc_2DEnergy(int channel,int num_frame);

void printTag(EventTag *tag,wave_t input);
void writeTag(EventTag **tag,wave_t input,FILE *fq);
void write_VADwav(wave_t input,double ***sample,EventTag tag[],char *filename);

FILE *fp,*fq,*fq2;
//double pre_tag;
//int pre_tag_frm;

int main(int argc,char *argv[]){
	fp=fopen(argv[1],"rb");
	char filename[1024]={};
	strcpy(filename,argv[1]);
	filename[strlen(filename)-4]='\0';
	strcat(filename,"_VAD.lab");
	fq=fopen(filename,"wb");
	filename[strlen(filename)-8]='\0';
	strcat(filename,"_VAD.frm");
	fq2=fopen(filename,"wb");

	char filename_temp[1024]={};
	strcpy(filename_temp,argv[1]);

	wave_t input;
	header_Ins(&input,fp);
    InitFrame(input);
	printf("sizeof \"%s\" is %d bytes\n",argv[1],input.Subchunk2Size);
	printf("Channels:%d \n",input.NumChannels);
    printf("Sample Rate:%d\n\n",input.SampleRate);

	int numOfFrame=input.Subchunk2Size/(sizeof(short)*OFFSET)+1;
	double ***sample=malloc_3DframeDBL(numOfFrame,input.NumChannels);
	int frame_num=framing(input,sample,fp,1);

	//save ori_data_mix
	double sum,i_sum;
	double ***mix_temp=malloc_3DframeDBL(numOfFrame,1);
	int i,j,k;
	for(i=0;i<frame_num;i++){
		for(j=0;j<WINDOW_FRAME_SIZE;j++){
			sum=i_sum=0;
			for(k=0;k<input.NumChannels;k++){
				//function framing do hamming windows, reverse it
				sum+=sample[k][i][2*j+1]/hamming[j];
				i_sum+=sample[k][i][2*j+2]/=hamming[j];
			}
			mix_temp[0][i][2*j+1]=sum/input.NumChannels;
			mix_temp[0][i][2*j+2]=i_sum/input.NumChannels;
		}
	}


	fft_frame(input,sample,frame_num,1);
	printf("Framem Number: %d\n",frame_num);
	//mix down the channels to channel 1
	double ***mix=malloc_3DframeDBL(numOfFrame,1);
	for(i=0;i<frame_num;i++){
		for(j=0;j<WINDOW_FRAME_SIZE;j++){
			sum=i_sum=0;
			for(k=0;k<input.NumChannels;k++){
				sum+=sample[k][i][2*j+1];
				i_sum+=sample[k][i][2*j+2];
			}
			mix[0][i][2*j+1]=sum/input.NumChannels;
			mix[0][i][2*j+1]=i_sum/input.NumChannels;
		}
	}

    //Energy **E=malloc_2DEnergy(input.NumChannels,frame_num);
	Energy **E=malloc_2DEnergy(1,frame_num);
	EventTag *tag;
	tag=endpointDetect(input,mix[0],E[0],frame_num);

	printf("Channel1 Tag:\n");
	printTag(tag,input);

	write_VADwav(input,mix_temp,tag,filename_temp);

	free_3DframeDBL(sample,numOfFrame,input.NumChannels);
	free_3DframeDBL(mix,numOfFrame,1);
	free_3DframeDBL(mix_temp,numOfFrame,1);
    free(E);
	free(tag);
    fclose(fp);
    fclose(fq);
	fclose(fq2);
	return 0;
}

void globalEnergy(wave_t input,double **sampleFrame_fq,Energy *E,int index){
	int subband_fq_begin=250;
	int subband_fq_end=3750;
    double smth_coef=0.8f;
    double thr_coef=0.75f;
	int kS=subband_fq_begin/(input.SampleRate/WINDOW_FRAME_SIZE),kT=subband_fq_end/(input.SampleRate/WINDOW_FRAME_SIZE);	//subband range from 250Hzz-375Hz

	E[index].energy=0;
	int i;
	for(i=kS;i<kT;i++)	//sub-band energy
		E[index].energy+=(sampleFrame_fq[index][2*i+1]*sampleFrame_fq[index][2*i+1]+sampleFrame_fq[index][2*i+2]*sampleFrame_fq[index][2*i+2]);	//maybe overflow
    E[index].energy=E[index].energy==0?0:10*log(E[index].energy);

    if(index==0){
        /*E[index].g_max=E[index].energy;
		E[index].g_min=E[index].energy;
		E[index].g_max_smooth=E[index].energy;
		E[index].g_min_smooth=E[index].energy;*/
        E[index].g_max=E[index].g_max_smooth=0;
        E[index].g_min=E[index].g_min_smooth=DBL_MAX;
	}
	else{
		E[index].g_max = E[index].energy > E[index-1].g_max ? E[index].energy : E[index-1].g_max;
		E[index].g_min = E[index].energy < E[index-1].g_min ? E[index].energy : E[index-1].g_min;
		E[index].g_max_smooth = (1-smth_coef)*E[index-1].g_max + smth_coef*E[index].g_max;
		E[index].g_min_smooth = (1-smth_coef)*E[index-1].g_min + smth_coef*E[index].g_min;
	}
	E[index].thr=E[index].g_min_smooth + thr_coef * (E[index].g_max_smooth-E[index].g_min_smooth);
}
EventTag* endpointDetect(wave_t input,double **sampleCH_fq,Energy *E, int frame_num){
	typedef enum{S0=0,S1,S2,S3,S4,S5,S6}FSM;
	FSM state=S0;
	int cnt=0,cnt_tag=0,I;
	const int CMAX=10,IMAX=5;	//frame number > CMAX =>speech; frame number < IMAX =>short noise

	EventTag *tag=(EventTag*)calloc(frame_num,sizeof(EventTag));
	int i;
	for(i=0;i<frame_num;i++){
		globalEnergy(input,sampleCH_fq,E,i);
		printf("State=S%d\n",state);
	    printf("frame[%3d] energy: %0.2g g_max = %0.2g g_min = %0.2g thr = %0.2g\n",i,E[i].energy,E[i].g_max_smooth,E[i].g_min_smooth,E[i].thr);
		switch(state){
			case S0:
				cnt_tag=cnt=I=0;
				i--;	//have to fix it later
				state=S1;
				break;
			case S1:
				state=S2;	//if device on
				i--;	//have to fix it later
				break;
			case S2:
				cnt=0;
				if(E[i].energy>=E[i].thr){
					tag[cnt_tag].s=i;	//start point
					//printf("IN TAG %d\n",i);
					state=S3;
				}
				else state=S2;
				break;
			case S3:
				cnt++,I=0;
				if(cnt>CMAX) state=S5;
				else if(E[i].energy>=E[i].thr && cnt<CMAX) state=S3;
				else if(E[i].energy<E[i].thr) state=S4;
				break;
			case S4:
				cnt++,I++;
				if(E[i].energy>=E[i].thr) state=S3;
				else if(I>IMAX){
					tag[cnt_tag].s=0;
					state=S2;
				}
				else if(E[i].energy<E[i].thr) state=S4;
				break;
			case S5:
				cnt++,I=0;
				if(E[i].energy>E[i].thr) state=S5;
				else if(E[i].energy<E[i].thr) state=S6;
				break;
			case S6:
				cnt++,I++;
				if(E[i].energy>=E[i].thr) state=S5;
				else if(I>IMAX){
					tag[cnt_tag++].t=i;
					//printf("OUT TAG %d\n",i);
					state=S2;
				}
				else if(E[i].energy<E[i].thr) state=S6;
				break;
		}
	}
    if(state==S5||state==S6)
        tag[cnt_tag++].t=i;
	return tag;
}


Energy **malloc_2DEnergy(int channel,int num_frame){
    int i;
    Energy **Array,*pData;
    Array=malloc(channel*sizeof(Energy*)+channel*num_frame*sizeof(Energy));
    for(i=0,pData=(Energy *)(Array+channel);i<channel;i++,pData+=num_frame)
        Array[i]=pData;
    return Array;
}
void printTag(EventTag *tag,wave_t input){
	int i;
	double pre_tag=0,tag_s,tag_t;
	int pre_tag_frm=0;
	for(i=0;tag[i].s||tag[i].t;i++){
		tag_s=(double)tag[i].s*OFFSET/input.SampleRate;
		tag_t=(double)tag[i].t*OFFSET/input.SampleRate;
		printf("[%2d] start frame_number: %5d end frame_number:%5d ts=%3.3lf sec tt=%3.3lf sec\n",i,tag[i].s,tag[i].t,tag_s,tag_t);
		/*if(i){
			fprintf(fq,"%lf %lf sli\n",pre_tag,tag_s);
			fprintf(fq2,"%d %d sli\n",pre_tag_frm*OFFSET,tag[i].s*OFFSET);
		}*/
		fprintf(fq,"%lf %lf sli\n",pre_tag,tag_s);
		fprintf(fq2,"%d %d sli\n",pre_tag_frm*OFFSET,tag[i].s*OFFSET);
		fprintf(fq,"%lf %lf A\n",tag_s,tag_t);
		fprintf(fq2,"%d %d A\n",tag[i].s*OFFSET,tag[i].t*OFFSET);

		pre_tag_frm=tag[i].t;
		pre_tag=tag_t;
	}

}
void write_VADwav(wave_t input,double ***sample,EventTag *tag,char *filename){
	//file open
	FILE *fqout;
	filename[strlen(filename)-4]='\0';
	strcat(filename,"_VAD.wav");
	fqout=fopen(filename,"wb");

	wave_t output=input;
	output.NumChannels=1;
	int sum=0;
	int i,j,k;
	int last_tag;
	for(i=0;tag[i].s||tag[i].t;++i){
		sum+=(tag[i].t-tag[i].s)*OFFSET;
	}
	last_tag=i-1;
	sum+=WINDOW_FRAME_SIZE-OFFSET;
	output.Subchunk2Size=sum*output.BitsPerSample;
	output.ChunkSize=output.Subchunk2Size+36;

	header_Write(&output,fqout);
	short temp;
	int output_size;
	for(i=0;tag[i].s||tag[i].t;++i){
		for(j=tag[i].s;j<tag[i].t;++j){
			output_size=(i==last_tag && j==tag[i].t-1)?WINDOW_FRAME_SIZE:OFFSET;
			for(k=0;k<output_size;++k){
				temp=(short)sample[0][j][2*k+1];
				fwrite(&temp,sizeof(short),1,fqout);
			}
		}
	}
	fclose(fqout);
}
