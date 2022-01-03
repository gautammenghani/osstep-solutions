#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

int ro=0,co=100;

int getFileSize(FILE *fp) {
  char c;
  int rows=0;
  for (c = getc(fp); c != EOF; c = getc(fp))
      if (c == '\n') 
          rows = rows + 1;
  rewind(fp);
  return rows;
}

bool filesAreSame(FILE *fp, FILE *fout, int finRows, char data[ro][co]) {
  int foutRows = getFileSize(fout);
  if (finRows !=foutRows) {
    return false;
  }

  char *line;
  size_t len=0;
  int l,i=0;
  while ((l = getline(&line, &len, fout)) > 0) {
    if (strcmp(line,data[i])!=0)  
      return false;
    i++;
  }
  return true;
}

int main(int argc, char **argv){
  int l;
  size_t len=0;
  char *line;
  if (argc>3) { 
    fprintf(stderr, "usage: reverse <input> <output>\n");
    exit(1);
  }
  else if(argc==1) {
    //read from stdin and write to stdout
    while ((l = getline(&line, &len, stdin)) > 1) {
          printf("%s", line);
    }
    exit(0);
  }
  
  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL) {
      fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
      exit(1);
  }

  // calculating the lines
  int rows=getFileSize(fp);
  ro=rows;
  char data[rows][100];
  int i=0;
  while ((l = getline(&line, &len, fp)) > 0) {
    strcpy(data[i],line);
    i++;
  }
  //if output file is not specified, write it to stdout
  if (argc==2) {
    for(i=rows-1;i>=0;--i) {
      printf("%s",data[i]);
    }
  }
  else {
    FILE *foutr = fopen(argv[2],"r+");
    if (foutr == NULL) {
      fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
      exit(1);
    }
    
    //if input and output files are same, exit(1)
    if (filesAreSame(fp, foutr, rows, data)) {
      fprintf(stderr, "reverse: input and output file must differ\n");
      exit(1);
    }
    //write it to file
    for(i=rows-1;i>=0;--i) {
      fwrite(&data[i],strlen(data[i]),1,foutr);
    }
    fclose(foutr);
  }
  fclose(fp);
  return 0;
}
