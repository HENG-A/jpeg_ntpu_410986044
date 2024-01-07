#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  FILE *fdim = fopen(argv[6], "wb");
  FILE *fr = fopen(argv[3], "wb");
  FILE *fg = fopen(argv[4], "wb");
  FILE *fb = fopen(argv[5], "wb");
  fprintf(fdim, "%d %d", width, height);
  for(int i=0; i<img_size*3; i+=3)
  {
    fprintf(fr, "%u ", bmp[i+2]);
    fprintf(fg, "%u ", bmp[i+1]);
    fprintf(fb, "%u ", bmp[i]);
  }
  fclose(fdim);
  fclose(fr);
  fclose(fg);
  fclose(fb);
  free(bmp);
  return 0;
}

int main(int argc, char* argv[])
{
  int mode;
  mode = atoi(argv[1]);
  //若輸入mode符合，執行對應function
  if(mode==0)  readBmp(argv); // BGR


  return 0;
}
