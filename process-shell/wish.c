#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
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

void printStruct(struct command cmd){
  printf("\n%s : %s ",cmd.commandName, *cmd.argv);
}

bool isBuiltInCommand(char *cmdName) {
  if(strcmp(cmdName, "cd")==0 || strcmp(cmdName, "path")==0 || strcmp(cmdName, "exit")==0)
    return true;
  else
    return false;
}

int executeBuiltInCommand(char *cmdName) {
  if (strcmp(cmdName, "cd")==0) {
    printf("cd cmdn\n");
  }
  else if(strcmp(cmdName, "path")==0) {
    printf("path cmdn\n");
  }
  else {
    //exit
    printf("exit cmdn\n");
    exit(0);
  }
  return 0;
}

int executeCommand(struct command cmd) {
  if (isBuiltInCommand(cmd.commandName))
    return executeBuiltInCommand(cmd.commandName);
    
  int rc = fork();
  int res;
  if (rc<0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  } 
  else if (rc==0) {
    //run the command in forked process
    res = execv(cmd.commandName, cmd.argv);
    if (res<0) 
      exit(1);
    else
      exit(0);
  }
  else {
    wait(&res);
  }
  return res;
}

struct command parseCommand(char *line) {
  char *token;
  char *command = strdup(line);
  char *commandName;
  char *argv[20];
  int i=1;
  struct command cmd;

  //remove trailing '\n'
  command[strlen(command)-1]='\0';

  //first arg needs to be the program name
  argv[0]="wish";

  commandName = strsep(&command, " ");
  while (1){
    token = strsep(&command, " ");
    if (token == NULL) {
      argv[i] = NULL;
      break;
    }
    argv[i] = token;
    i++;
  }

  /*if(strcmp(commandName, "path") == 0) {
    cmd={NULL, NULL, argv};
  }
  else*/
  cmd.commandName = commandName;
  cmd.argv = argv;

  return cmd;
}

int main(int argc, char **args) {
  char *line;
  int l,res;
  size_t len=0;
  struct command cmd;

  printf("wish> ");
  while ((l=getline(&line, &len, stdin))) {
    if (l>1) {
      cmd = parseCommand(line);
      //printStruct(cmd);
      res = executeCommand(cmd);
      if (res!=0)
        printf("command not found\n");
    }
    printf("wish> ");
  }

  return 0;
}
