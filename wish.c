#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

struct command {
  char *commandName;
  char **argv;
};

void printArgv(char **argv) {
  int i=0;
  while (argv[i]!=NULL) {
    printf("st : '%s'\n",argv[i]);
    i++;
  }
}
int executeCommand(struct command cmd) {
  //printArgv(cmd.argv);
  int rc = fork();
  if (rc<0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } 
  else if (rc==0) {
    printf("hello, I am child (pid:%d)\n", (int) getpid());
    int res = execv(cmd.commandName, cmd.argv);
    printf("res : %d",res);
    exit(0);
  }
  else {
    int rc_wait = wait(NULL);
    printf("\nhello, I am parent of %d (rc_wait:%d) (pid:%d)\n", rc, rc_wait, (int) getpid());
  }
  return -99;
}

struct command parseCommand(char *line) {
  char *token;
  char *command = strdup(line);
  char *commandName;
  char* argv[20];
  int i=0;

  //remove trailing '\n'
  command[strlen(command)-1]='\0';

  commandName = strsep(&command, " ");
  //printf("commandname: %s",commandName);
  while (1){
    token = strsep(&command, " ");
    if (token == NULL) {
      argv[i] = NULL;
      break;
    }
    //printf("'%s'",token);
    argv[i] = token;
    i++;
  }
  struct command cmd = {commandName, argv};
  return cmd;
}

int main(int argc, char **args) {
  char *line;
  int l,res;
  size_t len=0;
  struct command cmd;

  printf("wish> ");
  while ((l=getline(&line, &len, stdin))) {
    if (strcmp(line,"exit\n")==0)
      exit(0);

    if (l>1) {
      cmd = parseCommand(line);
      res = executeCommand(cmd);
      printf("commdn success ? : %d\n",res);
    }
    printf("wish> ");
  }

  return 0;
}
