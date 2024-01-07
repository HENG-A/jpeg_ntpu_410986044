#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BMP_HEADER_SIZE 14
#define BMP_INFO_HEADER_SIZE 40

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
  unsigned char padding_1[3];
  unsigned int paddingSize = (4 - (width * 3) % 4) % 4;

  // 紀錄檔案大小及data保留大小
  unsigned int filesize = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE + (width * height * 3) + (paddingSize * height);
  unsigned int dataOffset = BMP_HEADER_SIZE + BMP_INFO_HEADER_SIZE;

  // 初始化
  memset(header, 0, BMP_HEADER_SIZE);
  memset(infoHeader, 0, BMP_INFO_HEADER_SIZE);
  memset(padding, 0, 3);
  memset(padding_1, 0, 3);
  padding_1[0] = 196;
  padding_1[1] = 14;
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
  int tmp;
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

int main(int argc, char* argv[])
{
  int mode;
  mode = atoi(argv[1]);
  if(mode==0) saveBmp(argv);

  return 0;
}
