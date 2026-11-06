#define _POSIX_C_SOURCE 200809L
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
static volatile sig_atomic_t running=1;
static void stop(int n){(void)n;running=0;}
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec/1e9;}
static void limit(int type,rlim_t value){struct rlimit r={value,value};if(setrlimit(type,&r))_exit(126);}
int main(int argc,char**argv){if(argc!=3){fprintf(stderr,"Usage: app WORKERS TSV_JOB_FILE\nTrusted command and arguments separated by tabs, one job per line. CPU 2s, wall 10s, address space 128MiB. Not a security sandbox.\n");return 2;}char*end;long workers=strtol(argv[1],&end,10);if(*end||workers<1||workers>8)return 2;FILE*file=fopen(argv[2],"r");if(!file){perror("job file");return 1;}char*jobs[100];int count=0;char line[8192];while(fgets(line,sizeof line,file)){if(!strchr(line,'\n')&&!feof(file)){fprintf(stderr,"Line too long\n");fclose(file);return 2;}line[strcspn(line,"\r\n")]=0;if(!line[0])continue;if(count==100){fprintf(stderr,"Limit 100 jobs\n");fclose(file);return 2;}jobs[count++]=strdup(line);if(!jobs[count-1])return 1;}fclose(file);pid_t pids[8]={0};double starts[8]={0};int indexes[8]={0},next=0,done=0,failed=0;signal(SIGINT,stop);signal(SIGTERM,stop);while(running&&done<count){for(int slot=0;slot<workers&&next<count;slot++)if(pids[slot]==0){int index=next++;pid_t pid=fork();if(pid<0){perror("fork");running=0;failed=1;break;}if(pid==0){setpgid(0,0);limit(RLIMIT_CPU,2);limit(RLIMIT_AS,128*1024*1024);limit(RLIMIT_FSIZE,1024*1024);limit(RLIMIT_CORE,0);limit(RLIMIT_NOFILE,32);int fd=open("/dev/null",O_RDONLY);if(fd>=0){dup2(fd,STDIN_FILENO);close(fd);}char*args[33];char*save;int n=0;for(char*t=strtok_r(jobs[index],"\t",&save);t&&n<32;t=strtok_r(NULL,"\t",&save))args[n++]=t;args[n]=NULL;if(!n)_exit(126);execvp(args[0],args);_exit(127);}setpgid(pid,pid);pids[slot]=pid;starts[slot]=now();indexes[slot]=index;}
 for(int slot=0;slot<workers;slot++)if(pids[slot]>0){int status;pid_t result=waitpid(pids[slot],&status,WNOHANG);if(result==pids[slot]){int code=WIFEXITED(status)?WEXITSTATUS(status):128+WTERMSIG(status);fprintf(stderr,"job %d exit %d\n",indexes[slot]+1,code);failed|=code!=0;pids[slot]=0;done++;}else if(now()-starts[slot]>10){kill(-pids[slot],SIGKILL);}}
 struct timespec pause={0,100000000};nanosleep(&pause,NULL);}
 for(int slot=0;slot<workers;slot++){
  if(pids[slot]>0){kill(-pids[slot],SIGKILL);waitpid(pids[slot],NULL,0);failed=1;}
 }
 for(int i=0;i<count;i++){free(jobs[i]);}
 return failed||!running?1:0;
}
