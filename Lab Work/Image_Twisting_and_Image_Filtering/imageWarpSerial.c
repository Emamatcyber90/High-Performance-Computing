#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "omp.h"	

void warpSerial(int width, int height, int *inputImage, int *outputImage)
{
	int index, xp, yp;
	int tx = width/2, ty = height/2;
	int x, y; 
	float radius, theta, PI = 3.141527f;
	int xi, yi;
	int rmax = width/2;
	double start = omp_get_wtime();
	for(yp = 0; yp<height;yp++)
	{
		for(xp = 0; xp<width;xp++)
		{
			index = xp + yp * width;
			if(xp!=tx&&yp!=ty)
			{	radius = sqrtf((xp - tx) * (xp - tx) + (yp - ty) * (yp - ty));
				theta = atan2((xp-tx),(yp-ty))+PI*((rmax-radius)/rmax)/2;
				if(radius<=rmax)
					x=(int)(floor(tx+radius*cos(theta)));
				else
					x=xp;
				if(radius<=rmax)
					y=(int)(floor(ty+radius*sin(theta)));
				else
					y=yp;
				outputImage[index] = inputImage[x+y*width];
			}
 	
		}
	}	
	double stop = omp_get_wtime();
	printf("%lf\n", (stop-start));	
	
}

int bilinearlyInterpolate(int *inputImage, int width, int height, int x, int y)
{
	int x2,y2;
	
	int sumOfIntensities = 0;
	sumOfIntensities += inputImage[x + y * width];
	if(x + 1 < width)
	{
		x2 = x + 1;

	}
	else
	{
		x2 = x - 1;
	}
	sumOfIntensities+= inputImage[x2 + y * width];
		
	if(y + 1 <height)
	{
		y2 = y + 1;
	}
	else
	{
		y2 = y - 1;
	}
	sumOfIntensities += inputImage[x + y2 * width];
	sumOfIntensities += inputImage[x2 + y2 * width];
	sumOfIntensities /= 4;
	return sumOfIntensities;
	
}

int main(int argc, char** argv){

	FILE *fp= fopen("lena512.bmp", "rb");
	FILE *f = fopen("shaleenlena.bmp", "wb");

	if (fp == NULL){
		printf("Errortoopen");
		exit(0);
	}

	if (f == NULL){
		printf("Errortocreate");
		exit(0);
	}
	int temp;

	fseek(fp, 18,SEEK_SET);
	fread(&temp, sizeof(int), 1, fp);
	int width = temp;

	fseek(fp, 22,SEEK_SET);
	fread(&temp, sizeof(int), 1, fp);
	int height = temp;
	int MATRIXSIZE=height*width;
	fseek(fp, 34,SEEK_SET);
	fread(&temp, sizeof(int), 1, fp);
	
	fseek(fp, 0,SEEK_SET);

	int i=0, j, var;
	int re;
	int pixInit[temp];
	int pixFinal[temp];
	int *p=pixFinal;	
	j=0;
	// printf("Size: %d",temp);	
	while(++i<(54+temp)){

		fread(&re, sizeof(char), 1, fp);		
		if(i>=54)		
		{
			var=(int)re;
			//var=var/2;
			re=(char)var;
			pixInit[j++]=re;
			fwrite(&re, 1, sizeof(char), f);
		}		
		else
			fwrite(&re, 1, sizeof(char), f);

	}
	
	i=0;
	warpSerial(width,height,pixInit,pixFinal);
	
	char temp2;
	fseek(f,54,SEEK_SET);
	for(i=0;i<MATRIXSIZE;i++)
	{
		
		temp2=(char)pixFinal[i];
		fwrite(&temp2,sizeof(char),1,f);
		
	}
	
	fclose(fp);
	fclose(f);
	
}