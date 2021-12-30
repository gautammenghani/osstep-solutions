#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

char* compressLine(int len, char *line) {
  int i=0,j=0,cnt=0;
  char *output=(char *)malloc(100);
  char *ch=(char *)malloc(5);
  //aaaabb
  while(j<len) {
    i=j;
    j++;
    cnt=1;
    while(j<len && line[i]==line[j]){
      i++;
      j++;
      cnt++;
    }
    printf("output : %s\n",output);
    sprintf(ch, "%d", cnt);
    strncat(ch, &line[i],1);
    strcat(output,ch);
  }
  if(i!=j) { 
    sprintf(ch, "%d", cnt);
    strncat(ch,&line[len-1],1);
    strcat(output,ch);
   }
  return strcat(output,"\n");
}

int main(int argc, char **argv){
  int i,l;
  size_t len=0;
  char *line;
  FILE *fout=fopen("out.z","wb");
  if (argc==1) { 
    printf("wgrep: searchterm [file ...]\n");
    exit(1);
  }
  //char *pattern = argv[1]; 
  //int patternLen = strlen(pattern);
  for(i=1; i<argc;i++){
    //printf("\nbegin scan : i - %d",i);
    FILE *fp = fopen(argv[i], "r");
    if (fp == NULL) {
        printf("wgrep: cannot open file\n");
        exit(1);
    }
    // get the file size
    //fseek(fp, 0L, SEEK_END);
    // calculating the size of the file
    //long long size = ftell(fp);
    //rewind(fp);
    char *tLine;
    while ((l = getline(&line, &len, fp)) > 0) { 
            tLine = compressLine(l, line);
            printf("\nCompresssed lien : %s\n",tLine);
            fwrite(tLine, sizeof(line),1, fout);          
    }
    fclose(fp);
    fclose(fout);
  }
  return 0;
}
