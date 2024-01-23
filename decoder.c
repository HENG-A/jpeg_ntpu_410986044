#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BMP_HEADER_SIZE 14
#define BMP_INFO_HEADER_SIZE 40
#define	PI	3.1415926535897

int saveBmp(char* argv[])
{
  //分別讀取盪案
  FILE *fdim = fopen(argv[6], "r");
  FILE *fr = fopen(argv[3], "r");
  FILE *fg = fopen(argv[4], "r");
  FILE *fb = fopen(argv[5], "r");

  unsigned int width;
  unsigned int height;
  unsigned char *bmp;
  //定義pixel，並將檔案資料寫入
  fscanf(fdim, "%d %d", &width, &height);
  bmp = (unsigned char *)malloc((width) * (height) * 3 * sizeof(unsigned char));

  unsigned int temp;
  for(int i=0; i<width*height; i++)
  {
    fscanf(fb, "%u", &temp);
    bmp[i*3] = (unsigned char)temp;
    fscanf(fg, "%u", &temp);
    bmp[i*3+1] = (unsigned char)temp;
    fscanf(fr, "%u", &temp);
    bmp[i*3+2] = (unsigned char)temp;
  }
  fclose(fdim);
  fclose(fr);
  fclose(fg);
  fclose(fb);
  // 定義BMP標頭檔
  unsigned char header[BMP_HEADER_SIZE];
  unsigned char infoHeader[BMP_INFO_HEADER_SIZE];

  // 定義padding大小
  unsigned char padding[3];
  unsigned int paddingSize = (4 - (width * 3) % 4) % 4;

  // 紀錄檔案大小及data保留大小
  unsigned int filesize = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + (width * height * 3) + (paddingSize * height);
  unsigned int dataOffset = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE;

  // 初始化
  memset(header, 0, BMP_HEADER_SIZE);
  memset(infoHeader, 0, BMP_INFO_HEADER_SIZE);
  memset(padding, 0, 3);

  // BMP簽名檔
  header[0] = 'B';
  header[1] = 'M';

  // 輸入檔案大小
  header[2] = filesize & 0xff;
  header[3] = (filesize >> 8) & 0xff;
  header[4] = (filesize >> 16) & 0xff;
  header[5] = (filesize >> 24) & 0xff;

  // 保留字元，不能更動
  /*
  header[6] = 0;
  header[7] = 0;
  header[8] = 0;
  header[9] = 0;
  */

  // data offset區
  header[10] = dataOffset & 0xff;
  header[11] = (dataOffset >> 8) & 0xff;
  header[12] = (dataOffset >> 16) & 0xff;
  header[13] = (dataOffset >> 24) & 0xff;

  // info header區
  infoHeader[0] = BMP_INFO_HEADER_SIZE & 0xff;
  infoHeader[1] = (BMP_INFO_HEADER_SIZE >> 8) & 0xff;
  infoHeader[2] = (BMP_INFO_HEADER_SIZE >> 16) & 0xff;
  infoHeader[3] = (BMP_INFO_HEADER_SIZE >> 24) & 0xff;

  // width區
  infoHeader[4] = width & 0xff;
  infoHeader[5] = (width >> 8) & 0xff;
  infoHeader[6] = (width >> 16) & 0xff;
  infoHeader[7] = (width >> 24) & 0xff;

  // height區
  infoHeader[8] = height & 0xff;
  infoHeader[9] = (height >> 8) & 0xff;
  infoHeader[10] = (height >> 16) & 0xff;
  infoHeader[11] = (height >> 24) & 0xff;

  // planes
  infoHeader[12] = 1;
  infoHeader[13] = 0;

  // bits per pixel
  infoHeader[14] = 24;
  infoHeader[15] = 0;

  //fix infomation
  infoHeader[20] = 40;
  infoHeader[21] = 75;
  infoHeader[22] = 50;
  infoHeader[24] = 19;
  infoHeader[25] = 11;
  infoHeader[28] = 19;
  infoHeader[29] = 11;
  // open file
  FILE *fp = fopen(argv[2], "wb");

  // 寫入標頭檔
  fwrite(header, sizeof(unsigned char), BMP_HEADER_SIZE, fp);
  fwrite(infoHeader, sizeof(unsigned char), BMP_INFO_HEADER_SIZE, fp);

  // 開始寫入pixels
  for (int i = 0; i < height; i++)
  {
    // 寫入pixels
    fwrite(bmp + (width * 3 * i), sizeof(unsigned char), (width * 3), fp);
    // 寫入padding
    fwrite(padding, sizeof(unsigned char), paddingSize, fp);
  }
  // 關閉檔案
  fclose(fp);
  free(bmp);
  return 0;
}

int saveBmp1a(char* argv[])
{
  //分別讀取盪案
  FILE *fdim = fopen(argv[7], "r");
  FILE *f_Qt_Y = fopen(argv[4], "r");
  FILE *f_Qt_Cr = fopen(argv[5], "r");
  FILE *f_Qt_Cb = fopen(argv[6], "r");
  FILE *f_qF_Y = fopen(argv[8], "rb");
  FILE *f_qF_Cr = fopen(argv[9], "rb");
  FILE *f_qF_Cb = fopen(argv[10], "rb");

  int start_x;
  int start_y;
  float *q_Y, *q_Cb, *q_Cr;
  int *qt_Y, *qt_Cb, *qt_Cr;

  float	sum_Y,sum_Cb,sum_Cr,Cu,Cv;
  int	x,y,u,v;

  int quantizationMatrixY[8][8];
  int quantizationMatrixCr[8][8];
  int quantizationMatrixCb[8][8];

  unsigned int width;
  unsigned int height;
  unsigned char *bmp;
  //定義pixel，並將檔案資料寫入
  fscanf(fdim, "%d %d", &width, &height);
  int width_extend;
  int height_extend;
  height_extend = (int)(ceil(height/8.0)) * 8;
  width_extend = (int)(ceil(width/8.0)) * 8;
  q_Y = (float *)malloc((width_extend) * (height_extend) * sizeof(float));
  qt_Y = (int *)malloc((width_extend) * (height_extend) * sizeof(int));

  q_Cb = (float *)malloc((width_extend) * (height_extend) * sizeof(float));
  qt_Cb = (int *)malloc((width_extend) * (height_extend) * sizeof(int));

  q_Cr = (float *)malloc((width_extend) * (height_extend) * sizeof(float));
  qt_Cr = (int *)malloc((width_extend) * (height_extend) * sizeof(int));
  bmp = (unsigned char *)malloc((width) * (height) * 3 * sizeof(unsigned char));

  short int temp;
  for(int i=0; i<width_extend*height_extend; i++)
  {
    fread(&temp, sizeof(short int), 1, f_qF_Y);
    qt_Y[i] = (int)temp;
    fread(&temp, sizeof(short int), 1, f_qF_Cr);
    qt_Cr[i] = (int)temp;
    fread(&temp, sizeof(short int), 1, f_qF_Cb);
    qt_Cb[i] = (int)temp;
  }
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      fscanf(f_Qt_Y, "%d", &quantizationMatrixY[i][j]);
      fscanf(f_Qt_Cr, "%d", &quantizationMatrixCr[i][j]);
      fscanf(f_Qt_Cb, "%d", &quantizationMatrixCb[i][j]);
    }
  }
  fclose(fdim);
  fclose(f_Qt_Y);
  fclose(f_Qt_Cr);
  fclose(f_Qt_Cb);
  fclose(f_qF_Y);
  fclose(f_qF_Cr);
  fclose(f_qF_Cb);

  float *Y;
  float *Cb;
  float *Cr;
  Y = (float *)malloc((width) * (height) * sizeof(float));
  Cb = (float *)malloc((width) * (height) * sizeof(float));
  Cr = (float *)malloc((width) * (height) * sizeof(float));
  //IDCT
  for (start_x=0;start_x<width_extend;start_x+=8){
  for (start_y=0;start_y<height_extend;start_y+=8)
  {
    for (y=0;y<8;y++)
    {
      for (x=0;x<8;x++)
      {
        q_Y[(start_x+x)+(start_y+y)*width_extend] = qt_Y[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrixY[y][x];
        q_Cr[(start_x+x)+(start_y+y)*width_extend] = qt_Cr[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrixCr[y][x];
        q_Cb[(start_x+x)+(start_y+y)*width_extend] = qt_Cb[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrixCb[y][x];

      }
    }
  }
  }

  for (start_x=0;start_x<width_extend;start_x+=8){
  for (start_y=0;start_y<height_extend;start_y+=8)
  {
  for (u=0;u<8;++u)
    {
    for(v=0;v<8;++v)
    {
      sum_Y=0;
      sum_Cb=0;
      sum_Cr=0;
      for(y=0;y<8;y++)
      {
        for(x=0;x<8;x++)
        {
          if (y == 0) Cu = 1.0 / sqrt(2.0);else Cu = 1.0;
          if (x == 0) Cv = 1.0 / sqrt(2.0);else Cv = 1.0;
          sum_Y+=q_Y[(start_x+x)+(start_y+y)*width_extend]*Cv*Cu*cos((2.0*u+1)*y*PI/16.0)*cos((2.0*v+1)*x*PI/16.0);
          sum_Cb+=q_Cb[(start_x+x)+(start_y+y)*width_extend]*Cv*Cu*cos((2.0*u+1)*y*PI/16.0)*cos((2.0*v+1)*x*PI/16.0);
          sum_Cr+=q_Cr[(start_x+x)+(start_y+y)*width_extend]*Cv*Cu*cos((2.0*u+1)*y*PI/16.0)*cos((2.0*v+1)*x*PI/16.0);

        }
      }
      if(start_y+v<height && start_x+u<width)
      {
        Y[start_x+u+(start_y+v)*width] = 0.25*sum_Y;
        Cb[start_x+u+(start_y+v)*width] = 0.25*sum_Cb;
        Cr[start_x+u+(start_y+v)*width] = 0.25*sum_Cr;
      }


    }
    }
    //printf("%d\n",start_x+x+(start_y+y)*width);
  }
  //reverse array:
  //[ 1.08813928,  0.02376776,  1.64889711],
  //[ 1.08813928, -0.36588611, -0.76037116],
  //[ 1.08813928,  2.04338216,  0.05186507]
  }
  for(int i=0; i<(width) * (height); i++)
  {
    bmp[i*3+2] = (int)(round((double)(1.088139*(Y[i]-16)+0.023768*(Cb[i]-128)+1.648897*(Cr[i]-128))));
    bmp[i*3+1] = (int)(round((double)(1.088139*(Y[i]-16)-0.365886*(Cb[i]-128)-0.760371*(Cr[i]-128))));
    bmp[i*3] = (int)(round((double)(1.088139*(Y[i]-16)+2.043382*(Cb[i]-128)+0.051865*(Cr[i]-128))));
  }

  // 定義BMP標頭檔
  unsigned char header[BMP_HEADER_SIZE];
  unsigned char infoHeader[BMP_INFO_HEADER_SIZE];

  // 定義padding大小
  unsigned char padding[3];
  unsigned int paddingSize = (4 - (width * 3) % 4) % 4;

  // 紀錄檔案大小及data保留大小
  unsigned int filesize = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + (width * height * 3) + (paddingSize * height);
  unsigned int dataOffset = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE;

  // 初始化
  memset(header, 0, BMP_HEADER_SIZE);
  memset(infoHeader, 0, BMP_INFO_HEADER_SIZE);
  memset(padding, 0, 3);

  // BMP簽名檔
  header[0] = 'B';
  header[1] = 'M';

  // 輸入檔案大小
  header[2] = filesize & 0xff;
  header[3] = (filesize >> 8) & 0xff;
  header[4] = (filesize >> 16) & 0xff;
  header[5] = (filesize >> 24) & 0xff;

  // 保留字元，不能更動
  /*
  header[6] = 0;
  header[7] = 0;
  header[8] = 0;
  header[9] = 0;
  */

  // data offset區
  header[10] = dataOffset & 0xff;
  header[11] = (dataOffset >> 8) & 0xff;
  header[12] = (dataOffset >> 16) & 0xff;
  header[13] = (dataOffset >> 24) & 0xff;

  // info header區
  infoHeader[0] = BMP_INFO_HEADER_SIZE & 0xff;
  infoHeader[1] = (BMP_INFO_HEADER_SIZE >> 8) & 0xff;
  infoHeader[2] = (BMP_INFO_HEADER_SIZE >> 16) & 0xff;
  infoHeader[3] = (BMP_INFO_HEADER_SIZE >> 24) & 0xff;

  // width區
  infoHeader[4] = width & 0xff;
  infoHeader[5] = (width >> 8) & 0xff;
  infoHeader[6] = (width >> 16) & 0xff;
  infoHeader[7] = (width >> 24) & 0xff;

  // height區
  infoHeader[8] = height & 0xff;
  infoHeader[9] = (height >> 8) & 0xff;
  infoHeader[10] = (height >> 16) & 0xff;
  infoHeader[11] = (height >> 24) & 0xff;

  // planes
  infoHeader[12] = 1;
  infoHeader[13] = 0;

  // bits per pixel
  infoHeader[14] = 24;
  infoHeader[15] = 0;

  //fix infomation
  infoHeader[20] = 40;
  infoHeader[21] = 75;
  infoHeader[22] = 50;
  infoHeader[24] = 19;
  infoHeader[25] = 11;
  infoHeader[28] = 19;
  infoHeader[29] = 11;
  // open file

  FILE *fp = fopen(argv[2], "wb");

  // 寫入標頭檔
  fwrite(header, sizeof(unsigned char), BMP_HEADER_SIZE, fp);
  fwrite(infoHeader, sizeof(unsigned char), BMP_INFO_HEADER_SIZE, fp);
  // 開始寫入pixels
  for (int i = 0; i < height; i++)
  {
    // 寫入pixels
    fwrite(bmp + (width * 3 * i), sizeof(unsigned char), (width * 3), fp);
    // 寫入padding
    fwrite(padding, sizeof(unsigned char), paddingSize, fp);
  }
  // 關閉檔案
  fclose(fp);

  //讀取原始檔案 做SQNR
  FILE *fp2 = fopen(argv[3], "rb");
  if (!fp2)
    return 1;
  unsigned int width2;
  unsigned int height2;
  // 讀取高度
  fseek(fp2, 18, SEEK_SET);
  fread(&width2, sizeof(unsigned int), 1, fp2);

  // 讀取寬度
  fseek(fp2, 22, SEEK_SET);
  fread(&height2, sizeof(unsigned int), 1, fp2);

  // 定義paddingSize以符合BMP格式
  unsigned int paddingSize_o = (4 - ((width2) * 3) % 4) % 4;

  // 讀取 data offset
  fseek(fp2, 10, SEEK_SET);
  unsigned int dataOffset_o;
  fread(&dataOffset_o, sizeof(unsigned int), 1, fp2);

  // 讀取pixel資料
  fseek(fp2, dataOffset_o, SEEK_SET);
  unsigned char *bmp_o;
  bmp_o = (unsigned char *)malloc((width2) * (height2) * 3 * sizeof(unsigned char));
  int img_size_o = (width2) * (height2);
  for (int i = 0; i < (height2); i++)
  {
    //讀取pixel
    fread((bmp_o) + ((width2) * 3 * i), sizeof(unsigned char), (width2) * 3, fp2);

    // 跳過padding空白值
    fseek(fp2, paddingSize_o, SEEK_CUR);
  }
  // 關閉檔案
  fclose(fp2);
  double signal_power[3] = {0}; // R, G, B 通道的信號功率
  double noise_power[3] = {0};  // R, G, B 通道的噪聲功率
  // 遍歷每個像素
    for (int i = 0; i < img_size_o; ++i) {
            // 計算每個通道的信號功率和噪聲功率
            signal_power[0] += bmp_o[i*3+2] * bmp_o[i*3+2];
            noise_power[0] += (bmp[i*3+2] - bmp_o[i*3+2]) * (bmp[i*3+2] - bmp_o[i*3+2]);

            signal_power[1] += bmp_o[i*3+1] * bmp_o[i*3+1];
            noise_power[1] += (bmp[i*3+1] - bmp_o[i*3+1]) * (bmp[i*3+1] - bmp_o[i*3+1]);

            signal_power[2] += bmp_o[i*3] * bmp_o[i*3];
            noise_power[2] += (bmp[i*3] - bmp_o[i*3]) * (bmp[i*3] - bmp_o[i*3]);
    }

    // 計算並顯示每個通道的 SQNR
    if (noise_power[0] == 0) printf("Channel R: SQNR = 0 dB\n");
    else printf("Channel R: SQNR = %lf dB\n",10 * log10((double)signal_power[0] / (double)noise_power[0]));
    if (noise_power[1] == 0) printf("Channel G: SQNR = 0 dB\n");
    else printf("Channel R: SQNR = %lf dB\n",10 * log10((double)signal_power[1] / (double)noise_power[1]));
    if (noise_power[2] == 0) printf("Channel B: SQNR = 0 dB\n");
    else printf("Channel R: SQNR = %lf dB\n",10 * log10((double)signal_power[2] / (double)noise_power[2]));



  free(bmp);
  free(bmp_o);
  return 0;
}

int saveBmp1b(char* argv[])
{
  //分別讀取盪案
  FILE *fdim = fopen(argv[6], "r");
  FILE *f_Qt_Y = fopen(argv[3], "r");
  FILE *f_Qt_Cr = fopen(argv[4], "r");
  FILE *f_Qt_Cb = fopen(argv[5], "r");
  FILE *f_qF_Y = fopen(argv[7], "rb");
  FILE *f_qF_Cr = fopen(argv[8], "rb");
  FILE *f_qF_Cb = fopen(argv[9], "rb");
  FILE *f_eF_Y = fopen(argv[10], "rb");
  FILE *f_eF_Cr = fopen(argv[11], "rb");
  FILE *f_eF_Cb = fopen(argv[12], "rb");

  int start_x;
  int start_y;
  float *q_Y, *q_Cb, *q_Cr;
  float *ef_Y, *ef_Cb, *ef_Cr;
  int *qt_Y, *qt_Cb, *qt_Cr;

  float	sum_Y,sum_Cb,sum_Cr,Cu,Cv;
  int	x,y,u,v;

  int quantizationMatrixY[8][8];
  int quantizationMatrixCr[8][8];
  int quantizationMatrixCb[8][8];

  unsigned int width;
  unsigned int height;
  unsigned char *bmp;
  //定義pixel，並將檔案資料寫入
  fscanf(fdim, "%d %d", &width, &height);
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
  bmp = (unsigned char *)malloc((width) * (height) * 3 * sizeof(unsigned char));

  short int temp;
  float ftemp;
  for(int i=0; i<width_extend*height_extend; i++)
  {
    fread(&temp, sizeof(short int), 1, f_qF_Y);
    qt_Y[i] = (int)temp;
    fread(&temp, sizeof(short int), 1, f_qF_Cr);
    qt_Cr[i] = (int)temp;
    fread(&temp, sizeof(short int), 1, f_qF_Cb);
    qt_Cb[i] = (int)temp;
    fread(&ftemp, sizeof(float), 1, f_eF_Y);
    ef_Y[i] = ftemp;
    fread(&ftemp, sizeof(float), 1, f_eF_Cr);
    ef_Cr[i] = ftemp;
    fread(&ftemp, sizeof(float), 1, f_eF_Cb);
    ef_Cb[i] = ftemp;
  }
  for(int i=0; i<8; i++)
  {
    for(int j=0; j<8; j++)
    {
      fscanf(f_Qt_Y, "%d", &quantizationMatrixY[i][j]);
      fscanf(f_Qt_Cr, "%d", &quantizationMatrixCr[i][j]);
      fscanf(f_Qt_Cb, "%d", &quantizationMatrixCb[i][j]);
    }
  }
  fclose(fdim);
  fclose(f_Qt_Y);
  fclose(f_Qt_Cr);
  fclose(f_Qt_Cb);
  fclose(f_qF_Y);
  fclose(f_qF_Cr);
  fclose(f_qF_Cb);
  fclose(f_eF_Y);
  fclose(f_eF_Cr);
  fclose(f_eF_Cb);

  float *Y;
  float *Cb;
  float *Cr;
  Y = (float *)malloc((width) * (height) * sizeof(float));
  Cb = (float *)malloc((width) * (height) * sizeof(float));
  Cr = (float *)malloc((width) * (height) * sizeof(float));

  //IDCT
  for (start_x=0;start_x<width_extend;start_x+=8){
  for (start_y=0;start_y<height_extend;start_y+=8)
  {
    for (y=0;y<8;y++)
    {
      for (x=0;x<8;x++)
      {
        q_Y[(start_x+x)+(start_y+y)*width_extend] = ef_Y[(start_x+x)+(start_y+y)*width_extend] + qt_Y[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrixY[y][x];
        q_Cr[(start_x+x)+(start_y+y)*width_extend] = ef_Cr[(start_x+x)+(start_y+y)*width_extend] + qt_Cr[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrixCr[y][x];
        q_Cb[(start_x+x)+(start_y+y)*width_extend] = ef_Cb[(start_x+x)+(start_y+y)*width_extend] + qt_Cb[(start_x+x)+(start_y+y)*width_extend] * quantizationMatrixCb[y][x];

      }
    }
  }
  }
  //for(int i=0;i<20;i++) printf("%f %d %f\n",q_Y[i], qt_Y[i], ef_Y[i]);

  for (start_x=0;start_x<width_extend;start_x+=8){
  for (start_y=0;start_y<height_extend;start_y+=8)
  {
  for (u=0;u<8;++u)
    {
    for(v=0;v<8;++v)
    {
      sum_Y=0;
      sum_Cb=0;
      sum_Cr=0;
      for(y=0;y<8;y++)
      {
        for(x=0;x<8;x++)
        {
          if (y == 0) Cu = 1.0 / sqrt(2.0);else Cu = 1.0;
          if (x == 0) Cv = 1.0 / sqrt(2.0);else Cv = 1.0;
          sum_Y+=q_Y[(start_x+x)+(start_y+y)*width_extend]*Cv*Cu*cos((2.0*u+1)*y*PI/16.0)*cos((2.0*v+1)*x*PI/16.0);
          sum_Cb+=q_Cb[(start_x+x)+(start_y+y)*width_extend]*Cv*Cu*cos((2.0*u+1)*y*PI/16.0)*cos((2.0*v+1)*x*PI/16.0);
          sum_Cr+=q_Cr[(start_x+x)+(start_y+y)*width_extend]*Cv*Cu*cos((2.0*u+1)*y*PI/16.0)*cos((2.0*v+1)*x*PI/16.0);

        }
      }
      if(start_y+v<height && start_x+u<width)
      {
        Y[start_x+u+(start_y+v)*width] = 0.25*sum_Y;
        Cb[start_x+u+(start_y+v)*width] = 0.25*sum_Cb;
        Cr[start_x+u+(start_y+v)*width] = 0.25*sum_Cr;
      }


    }
    }
    //printf("%d\n",start_x+x+(start_y+y)*width);
  }
  //reverse array:
  //[ 1.08813928,  0.02376776,  1.64889711],
  //[ 1.08813928, -0.36588611, -0.76037116],
  //[ 1.08813928,  2.04338216,  0.05186507]
  }
  for(int i=0; i<(width) * (height); i++)
  {
    bmp[i*3+2] = (int)(round((double)(1.088139*(Y[i]-16)+0.023768*(Cb[i]-128)+1.648897*(Cr[i]-128))));
    bmp[i*3+1] = (int)(round((double)(1.088139*(Y[i]-16)-0.365886*(Cb[i]-128)-0.760371*(Cr[i]-128))));
    bmp[i*3] = (int)(round((double)(1.088139*(Y[i]-16)+2.043382*(Cb[i]-128)+0.051865*(Cr[i]-128))));
  }


  // 定義BMP標頭檔
  unsigned char header[BMP_HEADER_SIZE];
  unsigned char infoHeader[BMP_INFO_HEADER_SIZE];

  // 定義padding大小
  unsigned char padding[3];
  unsigned int paddingSize = (4 - (width * 3) % 4) % 4;

  // 紀錄檔案大小及data保留大小
  unsigned int filesize = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + (width * height * 3) + (paddingSize * height);
  unsigned int dataOffset = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE;

  // 初始化
  memset(header, 0, BMP_HEADER_SIZE);
  memset(infoHeader, 0, BMP_INFO_HEADER_SIZE);
  memset(padding, 0, 3);

  // BMP簽名檔
  header[0] = 'B';
  header[1] = 'M';

  // 輸入檔案大小
  header[2] = filesize & 0xff;
  header[3] = (filesize >> 8) & 0xff;
  header[4] = (filesize >> 16) & 0xff;
  header[5] = (filesize >> 24) & 0xff;

  // 保留字元，不能更動
  /*
  header[6] = 0;
  header[7] = 0;
  header[8] = 0;
  header[9] = 0;
  */

  // data offset區
  header[10] = dataOffset & 0xff;
  header[11] = (dataOffset >> 8) & 0xff;
  header[12] = (dataOffset >> 16) & 0xff;
  header[13] = (dataOffset >> 24) & 0xff;

  // info header區
  infoHeader[0] = BMP_INFO_HEADER_SIZE & 0xff;
  infoHeader[1] = (BMP_INFO_HEADER_SIZE >> 8) & 0xff;
  infoHeader[2] = (BMP_INFO_HEADER_SIZE >> 16) & 0xff;
  infoHeader[3] = (BMP_INFO_HEADER_SIZE >> 24) & 0xff;

  // width區
  infoHeader[4] = width & 0xff;
  infoHeader[5] = (width >> 8) & 0xff;
  infoHeader[6] = (width >> 16) & 0xff;
  infoHeader[7] = (width >> 24) & 0xff;

  // height區
  infoHeader[8] = height & 0xff;
  infoHeader[9] = (height >> 8) & 0xff;
  infoHeader[10] = (height >> 16) & 0xff;
  infoHeader[11] = (height >> 24) & 0xff;

  // planes
  infoHeader[12] = 1;
  infoHeader[13] = 0;

  // bits per pixel
  infoHeader[14] = 24;
  infoHeader[15] = 0;

  //fix infomation
  infoHeader[20] = 40;
  infoHeader[21] = 75;
  infoHeader[22] = 50;
  infoHeader[24] = 19;
  infoHeader[25] = 11;
  infoHeader[28] = 19;
  infoHeader[29] = 11;
  // open file

  FILE *fp = fopen(argv[2], "wb");

  // 寫入標頭檔
  fwrite(header, sizeof(unsigned char), BMP_HEADER_SIZE, fp);
  fwrite(infoHeader, sizeof(unsigned char), BMP_INFO_HEADER_SIZE, fp);
  // 開始寫入pixels
  for (int i = 0; i < height; i++)
  {
    // 寫入pixels
    fwrite(bmp + (width * 3 * i), sizeof(unsigned char), (width * 3), fp);
    // 寫入padding
    fwrite(padding, sizeof(unsigned char), paddingSize, fp);
  }
  // 關閉檔案
  fclose(fp);

  free(bmp);
  return 0;
}

int saveBmp1(int argc, char* argv[])
{
  if(argc==11) saveBmp1a(argv);
  if(argc==13) saveBmp1b(argv);
  return 0;
}

int main(int argc, char* argv[])
{
  int mode;
  mode = atoi(argv[1]);
  if(mode==0) saveBmp(argv);
  if(mode==1) saveBmp1(argc,argv);
  return 0;
}
