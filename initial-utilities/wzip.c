// 5/7 tests are passing
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

void compressLine(int len, char *line) {
  int i=0,j=0,cnt=1;
  while (j<len){
    i=j;
    j++;
    cnt=1;
    while(j<len && line[i]==line[j]) {
      cnt++;
      j++;
    }
    fwrite(&cnt, 4,1,stdout);
    fwrite(&line[i], 1,1,stdout);
  }
}

int main(int argc, char **argv){
  long i,l;
  size_t len=0;
  char *line;
  
  FILE *fout=fopen("out.txt","w");
  if (argc==1) { 
    printf("wzip: file1 [file2 ...]\n");
    exit(1);
  }
  for(i=1; i<argc;i++){
    FILE *fp = fopen(argv[i], "r");
    while ((l = getline(&line, &len, fp)) > 0) { 
      fwrite(line, strlen(line), 1, fout);                  
    }
    fclose(fp);
    
  }
  fclose(fout);
  FILE *fp = fopen("out.txt", "r");
  if (fp == NULL) {
      printf("wgrep: cannot open file\n");
      exit(1);
  }
  while ((l = getline(&line, &len, fp)) > 0) { 
    compressLine(l, line);                  
  }
  fclose(fp);

  return 0;
}
