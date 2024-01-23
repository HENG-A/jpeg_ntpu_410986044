#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define	PI	3.1415926535897

#define BMP_HEADER_SIZE 14
#define BMP_INFO_HEADER_SIZE 40

int readBmp(char* argv[])
{
  // 讀取檔案
  FILE *fp = fopen(argv[2], "rb");
  if (!fp)
    return 1;
  unsigned int width;
  unsigned int height;
  // 讀取高度
  fseek(fp, 18, SEEK_SET);
  fread(&width, sizeof(unsigned int), 1, fp);

  // 讀取寬度
  fseek(fp, 22, SEEK_SET);
  fread(&height, sizeof(unsigned int), 1, fp);

  // 定義paddingSize以符合BMP格式
  unsigned int paddingSize = (4 - ((width) * 3) % 4) % 4;

  // 讀取 data offset
  fseek(fp, 10, SEEK_SET);
  unsigned int dataOffset;
  fread(&dataOffset, sizeof(unsigned int), 1, fp);

  // 讀取pixel資料
  fseek(fp, dataOffset, SEEK_SET);
  unsigned char *bmp;
  bmp = (unsigned char *)malloc((width) * (height) * 3 * sizeof(unsigned char));
  int img_size = (width) * (height);
  for (int i = 0; i < (height); i++)
  {
    //讀取pixel
    fread((bmp) + ((width) * 3 * i), sizeof(unsigned char), (width) * 3, fp);

    // 跳過padding空白值
    fseek(fp, paddingSize, SEEK_CUR);
  }
  // 關閉檔案
  fclose(fp);

  //分別將資訊存至個別檔案
  FILE *fdim = fopen(argv[6], "w");
  FILE *fr = fopen(argv[3], "w");
  FILE *fg = fopen(argv[4], "w");
  FILE *fb = fopen(argv[5], "w");
  fprintf(fdim, "%d %d", width, height);
  for(int i=0; i<img_size*3; i+=3)
  {
    fprintf(fr, "%d ", bmp[i+2]);
    fprintf(fg, "%d ", bmp[i+1]);
    fprintf(fb, "%d ", bmp[i]);
  }
  fclose(fdim);
  fclose(fr);
  fclose(fg);
  fclose(fb);
  free(bmp);
  return 0;
}

int readBmp1(char* argv[])
{
  // 讀取檔案
  FILE *fp = fopen(argv[2], "rb");
  if (!fp)
    return 1;
  unsigned int width;
  unsigned int height;
  // 讀取高度
  fseek(fp, 18, SEEK_SET);
  fread(&width, sizeof(unsigned int), 1, fp);

  // 讀取寬度
  fseek(fp, 22, SEEK_SET);
  fread(&height, sizeof(unsigned int), 1, fp);

  // 定義paddingSize以符合BMP格式
  unsigned int paddingSize = (4 - ((width) * 3) % 4) % 4;

  // 讀取 data offset
  fseek(fp, 10, SEEK_SET);
  unsigned int dataOffset;
  fread(&dataOffset, sizeof(unsigned int), 1, fp);

  // 讀取pixel資料
  fseek(fp, dataOffset, SEEK_SET);
  unsigned char *bmp;
  bmp = (unsigned char *)malloc((width) * (height) * 3 * sizeof(unsigned char));
  int img_size = (width) * (height);
  for (int i = 0; i < (height); i++)
  {
    //讀取pixel
    fread((bmp) + ((width) * 3 * i), sizeof(unsigned char), (width) * 3, fp);

    // 跳過padding空白值
    fseek(fp, paddingSize, SEEK_CUR);
  }
  // 關閉檔案
  fclose(fp);
  //RGB2YCbCr

  float *Y;
  float *Cb;
  float *Cr;
  Y = (float *)malloc((width) * (height) * sizeof(float));
  Cb = (float *)malloc((width) * (height) * sizeof(float));
  Cr = (float *)malloc((width) * (height) * sizeof(float));

  for(int i=0; i<img_size; i++)
  {
    //  | Y  |   | 0.257 0.564 0.098 |  | R |   | 16 |
    //  | Cb | = | -.148 -.291 0.439 |．| G | + | 128|
    //  | Cr |   | 0.439 -.368 -.071 |  | B |   | 128|
    Y[i] = (float)bmp[i*3+2]*0.257+(float)bmp[i*3+1]*0.564+(float)bmp[i*3]*0.098 + 16.0;
    Cb[i] = (float)-bmp[i*3+2]*0.148-(float)bmp[i*3+1]*0.291+(float)bmp[i*3]*0.439 + 128.0;
    Cr[i] = (float)bmp[i*3+2]*0.439-(float)bmp[i*3+1]*0.368-(float)bmp[i*3]*0.071 + 128.0;
    //fprintf(fr, "%d ", bmp[i+2]);
    //fprintf(fg, "%d ", bmp[i+1]);
    //fprintf(fb, "%d ", bmp[i]);
  }

  int start_x;
  int start_y;
  float *q_Y, *q_Cb, *q_Cr;
  float *ef_Y, *ef_Cb, *ef_Cr;
  int *qt_Y, *qt_Cb, *qt_Cr;

  float	sum_Y,sum_Cb,sum_Cr,Cu,Cv;
  int	x,y,u,v;
  //for(int i=0;i<100;i++)printf("%f\n",Y[10000000+i]);

  int quantizationMatrix[8][8] = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 35, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
    };
  int width_extend;
  int height_extend;
  height_extend = (int)(ceil(height/8.0)) * 8;
  width_extend = (int)(ceil(width/8.0)) * 8;
  q_Y = (float *)malloc((width_extend) * (height_extend) * sizeof(float));
  qt_Y = (int *)malloc((width_extend) * (height_extend) * sizeof(int));
  ef_Y = (float *)malloc((width_extend) * (height_extend) * sizeof(float));

  q_Cb = (float *)malloc((width_extend) * (height_extend) * sizeof(float));
  qt_Cb = (int *)malloc((width_extend) * (height_extend) * sizeof(int));
  ef_Cb = (float *)malloc((width_extend) * (height_extend) * sizeof(float));

  q_Cr = (float *)malloc((width_extend) * (height_extend) * sizeof(float));
  qt_Cr = (int *)malloc((width_extend) * (height_extend) * sizeof(int));
  ef_Cr = (float *)malloc((width_extend) * (height_extend) * sizeof(float));

  for (start_x=0;start_x<width_extend;start_x+=8){
  for (start_y=0;start_y<height_extend;start_y+=8){
  for (u=0;u<8;u++)
  {
  	for (v=0;v<8;v++)
		{

			sum_Y=0;
      sum_Cb=0;
      sum_Cr=0;
			for (x=0;x<8;x++)
      {
        //printf("x: %d\n",x);
				for (y=0;y<8;y++)
				{
          /*printf("y: %d\n",y);
          if(start_y==1208){
            printf("debug:%f\n", Y[start_x+x+(start_y+y)*width]);
            printf("debug:%d %f %f %f\n", start_x+x+(start_y+y)*width, cos(((2.0*x+1)*u*PI)/16.0), cos(((2.0*y+1)*v*PI)/16.0), Y[start_x+x+(start_y+y)*width]);
            printf("debug:%d %f %f %f\n", start_x+x+(start_y+y)*width, cos(((2.0*x+1)*u*PI)/16.0), cos(((2.0*y+1)*v*PI)/16.0), Cb[start_x+x+(start_y+y)*width]);
            printf("debug:%d %f %f %f\n", start_x+x+(start_y+y)*width, cos(((2.0*x+1)*u*PI)/16.0), cos(((2.0*y+1)*v*PI)/16.0), Cr[start_x+x+(start_y+y)*width]);
          }*/

          if(start_y+y<height && start_x+x<width)
          {
            sum_Y=sum_Y+Y[start_x+x+(start_y+y)*width]*cos(((2.0*x+1)*u*PI)/16.0)*cos(((2.0*y+1)*v*PI)/16.0);
            sum_Cb=sum_Cb+Cb[start_x+x+(start_y+y)*width]*cos(((2.0*x+1)*u*PI)/16.0)*cos(((2.0*y+1)*v*PI)/16.0);
            sum_Cr=sum_Cr+Cr[start_x+x+(start_y+y)*width]*cos(((2.0*x+1)*u*PI)/16.0)*cos(((2.0*y+1)*v*PI)/16.0);
          }
				}
      }
			if (u==0) Cu=1/sqrt(2); else Cu=1;
			if (v==0) Cv=1/sqrt(2); else Cv=1;
  		q_Y[(start_x+v)+(start_y+u)*width_extend]=1.0/4.0*Cu*Cv*sum_Y;
      q_Cb[(start_x+v)+(start_y+u)*width_extend]=1.0/4.0*Cu*Cv*sum_Cb;
      q_Cr[(start_x+v)+(start_y+u)*width_extend]=1.0/4.0*Cu*Cv*sum_Cr;
      //printf("%f %f\n",sum, q_Y[(start_x+v)+(start_y+u)*width]);
			//printf("%f %f \n",norm[u][v], (1.0/4.0*Cu*Cv*sum/norm[u][v]));
		}
    //printf("\n");
		//printf("%d\n",out_Y[u+(v-1)*width]);
	}
  }
  }
  //Qlize
  for (start_x=0;start_x<width_extend;start_x+=8){
  for (start_y=0;start_y<height_extend;start_y+=8)
  {
    for (x=0;x<8;x++)
    {
      for (y=0;y<8;y++)
      {
          qt_Y[(start_x+x)+(start_y+y)*width_extend] = (int)(q_Y[(start_x+x)+(start_y+y)*width_extend] / quantizationMatrix[y][x]);
          ef_Y[(start_x+x)+(start_y+y)*width_extend] = q_Y[(start_x+x)+(start_y+y)*width_extend] - qt_Y[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrix[y][x];
          qt_Cb[(start_x+x)+(start_y+y)*width_extend] = (int)(q_Cb[(start_x+x)+(start_y+y)*width_extend] / quantizationMatrix[y][x]);
          ef_Cb[(start_x+x)+(start_y+y)*width_extend] = q_Cb[(start_x+x)+(start_y+y)*width_extend] - qt_Cb[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrix[y][x];
          qt_Cr[(start_x+x)+(start_y+y)*width_extend] = (int)(q_Cr[(start_x+x)+(start_y+y)*width_extend] / quantizationMatrix[y][x]);
          ef_Cr[(start_x+x)+(start_y+y)*width_extend] = q_Cr[(start_x+x)+(start_y+y)*width_extend] - qt_Cr[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrix[y][x];

      }
    }
  }
  }
  //分別將資訊存至個別檔案
  FILE *fdim = fopen(argv[6], "w");
  FILE *f_Qt_Y = fopen(argv[3], "w");
  FILE *f_Qt_Cr = fopen(argv[4], "w");
  FILE *f_Qt_Cb = fopen(argv[5], "w");
  FILE *f_qF_Y = fopen(argv[7],"wb");
  FILE *f_qF_Cr = fopen(argv[8],"wb");
  FILE *f_qF_Cb = fopen(argv[9],"wb");
  FILE *f_eF_Y = fopen(argv[10],"wb");
  FILE *f_eF_Cr = fopen(argv[11],"wb");
  FILE *f_eF_Cb = fopen(argv[12],"wb");
  fprintf(fdim, "%d %d", width, height);
  for(y=0;y<8;y++)
  {
    for(x=0;x<8;x++)
    {
      fprintf(f_Qt_Y, "%d ", quantizationMatrix[y][x]);
      fprintf(f_Qt_Cb, "%d ", quantizationMatrix[y][x]);
      fprintf(f_Qt_Cr, "%d ", quantizationMatrix[y][x]);
    }
    fprintf(f_Qt_Y, "\n");
    fprintf(f_Qt_Cb, "\n");
    fprintf(f_Qt_Cr, "\n");
  }
  short int stemp;
  for(y=0;y<height_extend;y++)
  {
    for(x=0;x<width_extend;x++)
    {
      stemp = (short int)qt_Y[x+y*width_extend];
      fwrite(&stemp,sizeof(short int),1,f_qF_Y);
      stemp = (short int)qt_Cr[x+y*width_extend];
      fwrite(&stemp,sizeof(short int),1,f_qF_Cr);
      stemp = (short int)qt_Cb[x+y*width_extend];
      fwrite(&stemp,sizeof(short int),1,f_qF_Cb);

      fwrite(&(ef_Y[x+y*width_extend]),sizeof(float),1,f_eF_Y);
      fwrite(&(ef_Cr[x+y*width_extend]),sizeof(float),1,f_eF_Cr);
      fwrite(&(ef_Cb[x+y*width_extend]),sizeof(float),1,f_eF_Cb);
    }
  }
  //SQNR
  double Y_signal_power[64] = {0}; // 用於存儲每個頻率點的信號功率
  double Y_noise_power[64] = {0};  // 用於存儲每個頻率點的噪聲功率

  double Cr_signal_power[64] = {0}; // 用於存儲每個頻率點的信號功率
  double Cr_noise_power[64] = {0};  // 用於存儲每個頻率點的噪聲功率

  double Cb_signal_power[64] = {0}; // 用於存儲每個頻率點的信號功率
  double Cb_noise_power[64] = {0};  // 用於存儲每個頻率點的噪聲功率

  double signal;
  double noise;
  // 遍歷每個 8x8 DCT 塊
  for (start_y = 0; start_y < height; start_y += 8) {
      for (start_x = 0; start_x < width; start_x += 8) {
          // 處理每個塊內的元素
          for (int i = 0; i < 8; ++i) {
              for (int j = 0; j < 8; ++j) {
                  int idx = i * 8 + j; // 計算頻率點的索引
                  signal = q_Y[start_y + i + (start_x + j)*width];
                  noise = ef_Y[start_y + i + (start_x + j)*width];

                  Y_signal_power[idx] += signal * signal;
                  Y_noise_power[idx] += noise * noise;

                  signal = q_Cr[start_y + i + (start_x + j)*width];
                  noise = ef_Cr[start_y + i + (start_x + j)*width];

                  Cr_signal_power[idx] += signal * signal;
                  Cr_noise_power[idx] += noise * noise;

                  signal = q_Cb[start_y + i + (start_x + j)*width];
                  noise = ef_Cb[start_y + i + (start_x + j)*width];

                  Cb_signal_power[idx] += signal * signal;
                  Cb_noise_power[idx] += noise * noise;
              }
          }
      }
  }
  double sqnr;
  printf("Y Frequency SQNR:\n");
  for (int k = 0; k < 64; ++k) {
    if (Y_noise_power[k] == 0) sqnr = 0; // 防止除以零
    else sqnr = 10.0 * log10(Y_signal_power[k] / Y_noise_power[k]);
    printf("Frequency %d: SQNR = %lf dB\n", k + 1, sqnr);
  }

  printf("Cr Frequency SQNR:\n");
  for (int k = 0; k < 64; ++k) {
    if (Cr_noise_power[k] == 0) sqnr = 0; // 防止除以零
    else sqnr = 10.0 * log10(Cr_signal_power[k] / Cr_noise_power[k]);
    printf("Frequency %d: SQNR = %lf dB\n", k + 1, sqnr);
  }

  printf("Cb Frequency SQNR:\n");
  for (int k = 0; k < 64; ++k) {
    if (Cb_noise_power[k] == 0) sqnr = 0; // 防止除以零
    else sqnr = 10.0 * log10(Cb_signal_power[k] / Cb_noise_power[k]);
    printf("Frequency %d: SQNR = %lf dB\n", k + 1, sqnr);
  }
  //for (int i=0;i<50;i++)printf("%f %d %f\n",q_Y[10000000+i],qt_Y[10000000+i],ef_Y[10000000+i]);
  return 0;
}

int main(int argc, char* argv[])
{
  int mode;
  mode = atoi(argv[1]);
  //若輸入mode符合，執行對應function
  if(mode==0)  readBmp(argv); // BGR
  if(mode==1)  readBmp1(argv); // BGR

  return 0;
}
