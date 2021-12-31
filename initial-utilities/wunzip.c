#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char **argv){
  int i;
  char line[1];
  if (argc==1) { 
    printf("wunzip: file1 [file2 ...]\n");
    exit(1);
  }
  for(i=1; i<argc;++i){
    FILE *fp = fopen(argv[i], "rb");
    if (fp == NULL) {
        printf("wcat: cannot open file\n");
        exit(1);
    }
    int count=0,i=0;
    while (fread(&count, 4, 1, fp)) {
      fread(line, 1, 1, fp);
      for(i=0;i<count;i++)
        printf("%s",line);
    }
    fclose(fp);
  }
  return 0;
}
