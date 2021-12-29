#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

bool stringFound(char *line, char *pattern, int lineLen, int patternLen) {
  //char c;
  int i=0,j,k;
  while(i<lineLen) {
    if (pattern[0]==line[i]) {
      // match the pattern
      j=1;
      k=1;
      while (j<patternLen && k<lineLen && pattern[j]==line[i+k]) {
        j++;
        k++;
      }
      if(j==patternLen)
        return true;
    }
    i++;
  }
  return false;
}

bool isFileName(char *fileName) {
  int i=0;
  int l=strlen(fileName);
  while (i<l) {
    if(fileName[i]=='.')
      return true;
    i++;
  }
  return false;
}

int main(int argc, char **argv){
  int i,l;
  size_t len=0;
  char *line;
  if (argc==1) { 
    printf("wgrep: searchterm [file ...]\n");
    exit(1);
  }
  else if(argc==2) {
    //check if only filename is passed
    if (isFileName(argv[1])) {exit(1);}
    //only pattern is passed, read from stdin
    char *pattern = argv[1]; 
    while ((l = getline(&line, &len, stdin)) > 1) {
        if (stringFound(line, pattern, l, strlen(pattern))) {
          printf("%s", line);
        }
    }
  }
  char *pattern = argv[1]; 
  int patternLen = strlen(pattern);
  for(i=2; i<argc;i++){
    //printf("\nbegin scan : i - %d",i);
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
        printf("wgrep: cannot open file\n");
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
    while ((l = getline(&line, &len, fp)) > 0) { 
      if (l>1) {
        if (stringFound(line, pattern, l, patternLen)) {
          printf("%s", line);
        }
      }
    }
    fclose(fp);
    free(str);
  }
  return 0;
}
