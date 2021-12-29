#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(int argc, char **argv){
  int i;
  for(i=0; i<argc-1;++i){
    FILE *fp = fopen(argv[i+1], "r");
    if (fp == NULL) {
        printf("wcat: cannot open file\n");
        exit(1);
    }
    // get the file size
    fseek(fp, 0L, SEEK_END);
    // calculating the size of the file
    long long size = ftell(fp);
    rewind(fp);
    char *str = calloc(1, size+1);
    if (!str) {
      fclose(fp);
      printf("\nBuffer allocation failed");
      exit(1);
    }
    fread(str, size, 1, fp);
    printf("%s",str);
    fclose(fp);
    free(str);
  }
  return 0;
}
