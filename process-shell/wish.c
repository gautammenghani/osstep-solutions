#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<sys/wait.h>
#include <fcntl.h>

struct command {
  char *commandName;
  char *argv[10];
};

char *path[10]={"/bin", NULL};

void printArgv(char **argv) {
  int i=0;
  while (argv[i]!=NULL) {
    printf("element: '%s'\n",argv[i]);
    i++;
  }
}

void printPath(){
  int i=0;
  while(path[i]) {
    printf("%d : %s\n",i, path[i]);
    i++;
  }
}

bool isCommandRedirected(char **argv) {
  int i=0;
  while(argv[i]) {
    if(strcmp(argv[i], ">")==0)
      return true;
    i++;
  }
  return false;
}

char* getFileNameFromCommand(char **argv){
  int i=0;
  while (argv[i]) {
    if (strcmp(argv[i],">")==0) {
      if(argv[i+1]) {
        argv[i] = NULL;
        return argv[i+1];
      }
    }
    i++;
  }
  return NULL;
}

bool isBuiltInCommand(char *cmdName) {
  if(strcmp(cmdName, "cd")==0 || strcmp(cmdName, "path")==0 || strcmp(cmdName, "exit")==0)
    return true;
  else
    return false;
}

int executeBuiltInCommand(struct command cmd) {
  int i=0;
  if (strcmp(cmd.commandName, "cd")==0) {
    //if number(args) is not 1, throw error
    i=1;
    while(cmd.argv[i])
      i++;
    if (i!=2) {
      fprintf(stderr, "cd has wrong number of args\n");
      return 1;
    }
    if(chdir(cmd.argv[1])==0) {
      printf("Current dir changed to : %s\n", cmd.argv[1]);
    }
    else {
      printf("Could not change dir\n");
      return 1;
    }
  }
  else if(strcmp(cmd.commandName, "path")==0) {
    if (cmd.argv[1]==NULL) {
      //path should be blank
      path[0]=NULL;
    }
    else {
      //set path to specified path
      i=0;
      while(cmd.argv[i+1]){
        path[i]=cmd.argv[i+1];
        i++;
      }
      path[i]=NULL;
    }
  }
  else 
    exit(0);
  return 0;
}

char* commandCanExecute(char *cmdName) {
  int i=0;
  char *absPath;
  while(path[i]!=NULL){
    absPath = strdup(path[i]);
    strcat(absPath, "/");
    strcat(absPath, cmdName);
    if(access(absPath, X_OK)==0) {
      return absPath;
    }
    i++;
  } 
  return NULL;
}

int execute(char *absPath, struct command cmd, bool isRedirection) {
  int fd=-1;
  int rc = fork();
  int res=0;
  char *fileName=NULL;
  if (rc<0) 
    exit(1);
   
  else if (rc==0) {
    //run the command in forked process
    if (isRedirection) {      
      //get the output filename
      fileName = getFileNameFromCommand(cmd.argv);
      printf("filename : %s\n",fileName);
      if(!fileName)
        return 1;
      fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
      dup2(fd, 1);
    }
    res = execv(absPath, cmd.argv);
    close(fd);
    if (res<0) 
      exit(1);
    else
      exit(0);
  }
  else 
    wait(&res);
  return res;
}

int executeCommand(struct command cmd) {
  bool isRedirection = false;

  if (isBuiltInCommand(cmd.commandName))
    return executeBuiltInCommand(cmd);
    
  //check if user command is found and can be executed
  char *absPath = commandCanExecute(cmd.commandName);
  if (!absPath) 
    return 1;

  //check if user wants to redirect output to a file
  isRedirection = isCommandRedirected(cmd.argv);

  //run the command
  return execute(absPath, cmd, isRedirection);
}

struct command parseCommand(char *line) {
  char *token;
  char *command = strdup(line);
  char *commandName;
  char *args[20];
  int i=1;
  struct command cmd;

  //remove trailing '\n'
  command[strlen(command)-1]='\0';

  //first arg needs to be the program name
  args[0]="wish";

  commandName = strsep(&command, " ");
  while (1){
    token = strsep(&command, " ");
    if (token == NULL) {
      args[i] = NULL;
      break;
    }
    args[i]=token;
    i++;
  }
  memcpy(cmd.argv, args, sizeof args);
  cmd.commandName = commandName;
  return cmd;
}

int splitCommands(char *line, char **lines) {
  int i=1;
  char *command;
  command = strsep(&line,"&"); 
  lines[0]=command;
  while (1) {
    command = strsep(&line,"&"); 
    if (!command)
      break;

    if(strlen(command)==1)  
      return 1;
    //strip leading white space from each command
    strsep(&command, " ");
    lines[i]=command;
    i++;
  }
  if (strlen(lines[i-1])==0)
    return 1;
  lines[i]=NULL;
  return (i>0?0:1); 
}

bool batchMode(char **args) {
  return false;
}

bool inputHasMultipleCommands(char *line) {
  return true;
}

int main(int argc, char **args) {
  char *line;
  char *parallelCmnds[20];
  int l, res;
  size_t len=0;
  struct command cmd;
  char error_message[30] = "An error has occurred\n";
  //struct command *cptr = &cmd;

  if (batchMode(args)) {

  }
  else {
    printf("wish> ");
    while ((l=getline(&line, &len, stdin))) {
      if (l>1) {
        if (inputHasMultipleCommands(line)) {
          int status = splitCommands(line, parallelCmnds);
          if(status==1) {
              write(STDERR_FILENO, error_message, strlen(error_message)); 
              exit(1);
          }
          int i=0;
          while (parallelCmnds[i]) {
            cmd = parseCommand(parallelCmnds[i]);
            res = executeCommand(cmd);
            if(res!=0)
              write(STDERR_FILENO, error_message, strlen(error_message)); 
            i++;
          }
        }
        else {
          cmd = parseCommand(line);
          res = executeCommand(cmd);
          if(res!=0)
            write(STDERR_FILENO, error_message, strlen(error_message)); 
        }
      }
      printf("wish> ");
    }
  }

  return 0;
}
