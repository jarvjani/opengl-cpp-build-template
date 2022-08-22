/*This is a small build program that compiles all the source files listed in source-list.txt individually.
* Object files will be put to build/obj/ folder, with .o appended to end of the file name, eg. main.c.o
* Compiler will be using given compiler flags from compiler-flags.txt 
* Linking will be done after all sources are compiled and flags from linker-flags.txt will be used. 
* Reason why i am not using make or cmake is that i have better control by writing the whole build program myself.
* And because i hate all the build systems. 
*/
#include "stdio.h"
#include "stdint.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/wait.h"
#include "sys/stat.h"
#include <sys/types.h>
#include "dirent.h"
#include "errno.h"
#include "libgen.h"

#define MAX_COMPILERFLAGS 100
#define MAX_LINKERFLAGS 100
#define MAX_SOURCEFILES 100
#define MAX_LIBRARYFILES 100
#define COMPILER "g++"
#define EXECUTABLE_NAME "Program"
#define MAX_COMPILER_ARGS 200

int getFilelines(FILE *file,char* lines[], char* fileBuffer ){
   int lineCount=0, size=0, i=0;

   fseek(file, 0, SEEK_END); // go to the end of file
   size=ftell(file);  // get size
   if (size < 1 ){
     perror("File was empty\n");
     exit(EXIT_FAILURE);
   }
   fileBuffer = calloc(size+1,sizeof(char)); // allocate and clear enoung memory
   fseek(file, 0, SEEK_SET); // seek back to beginning
   fread(fileBuffer,size,sizeof(char),file); // read the  to buffer

   lines[lineCount]= &fileBuffer[i];
   lineCount++;
   for (i = 0; i < size; i++){
      if(fileBuffer[i]=='\n'){
         fileBuffer[i]= 0;         
         lines[lineCount] =  &fileBuffer[i+1];
         lineCount++;
      }
   }
   return lineCount;
} 
void print_array (char **strings, size_t nptrs)
{
    for (size_t i = 0; i < nptrs; i++)
        printf("%s ", strings[i]);
    putchar ('\n');
}

int main(){
   FILE *pCompilerFlagsFile,
      *pLinkerFlagsFile,
      *pSourceListFile,
      *pLibraryListFile;
   pCompilerFlagsFile=fopen("build/compile-flags.txt","r");
   pLinkerFlagsFile=fopen("build/linker-flags.txt","r");
   pSourceListFile=fopen("build/source-list.txt","r");
   pLibraryListFile=fopen("build/library-list.txt","r");

   if(pCompilerFlagsFile == NULL || pLinkerFlagsFile == NULL || pSourceListFile == NULL || pLibraryListFile == NULL ){
     perror("Error opening files\n"); 
     exit(EXIT_FAILURE); // leaves open file handles...
   }

   char* compilerFlags[MAX_COMPILERFLAGS];
   char* linkerFlags[MAX_LINKERFLAGS];
   char* sourcelist[MAX_SOURCEFILES];
   char* librarylist[MAX_LIBRARYFILES];
   char* compilerFlagsBuffer;
   char* linkerFlagsBuffer;
   char* sourcelistBuffer;
   char* librarylistBuffer;

   /* Read files to memory, get lines */
   int nCompilerFlags = getFilelines(pCompilerFlagsFile,compilerFlags,compilerFlagsBuffer);
   int nLinkerFlags  = getFilelines(pLinkerFlagsFile,linkerFlags,linkerFlagsBuffer);
   int nSourceFiles = getFilelines(pSourceListFile,sourcelist,sourcelistBuffer);
   int nLibraryFiles = getFilelines(pLibraryListFile,librarylist,librarylistBuffer);

   /* create compiler args */
   char *args[MAX_COMPILER_ARGS]={COMPILER,NULL};
   int i , j=1;
   for (i=0; i < nCompilerFlags; i++){
      args[j]=compilerFlags[i];
      j++;
   }
   /*Lets start compiling all the files */
   char outname[128][MAX_SOURCEFILES]={{0},{0}},
   dasho[3]="-o", 
   outfolder[128]="build/obj/";
   for(i=0; i < nSourceFiles; i++ ){
      /* get name of the output file */
      memcpy(outname[i],outfolder,10);
      strcat(outname[i],sourcelist[i]);
      strcat(outname[i],".o");
      
      /* get output folder name, create it if it doesn't exists */
      strcpy(outfolder,outname[i]);
      dirname(outfolder);
      DIR* dir = opendir(outfolder);
      if (dir) {
         /* Directory exists. */
         closedir(dir);
      } else if (ENOENT == errno) {
         /* Directory does not exist. */
         char command[128]="mkdir -p ";
         strcat(command,outfolder);
         int ret = system(command);
         if(ret){
            printf("error creating folder: %s",outfolder);
            exit(EXIT_FAILURE);
         }
      } else {
         printf("error creating folder: %s",outfolder);
         exit(EXIT_FAILURE);
      }
      /* create rest of the command, fork new process that runs execution of the command.
      This creates as many processes as source files, runs in parallel  */
      j=nCompilerFlags+1; // return to argument after compiler flags
      args[j] = sourcelist[i];
      j++;
      args[j] =  dasho;
      j++;
      args[j]=outname[i];
      j++;
      {
         pid_t forkrank;
         forkrank=fork();
         if(forkrank == 0 ){
            //printf("child process is executing\n");
            exit(execvp(args[0],args));

         }else if(forkrank != -1){
            print_array(args,j);
            //printf("parent ran the loop...\n");
         }else{
            perror("fork error\n");
         }
      }
   }
   //printf("parent waiting...\n");
   wait(NULL);

   /*Compiling done, lets link*/
   /*create link command, copy the linking flags to args*/
   j=1;
   args[j]=dasho;
   j++;
   strcpy(args[j],EXECUTABLE_NAME);
   j++;

   for (i=0; i < nLinkerFlags; i++){
      args[j]=linkerFlags[i];
      j++;
   }
   for (i=0; i<nSourceFiles; i++){
      args[j]=outname[i];
      j++;
   }
   for (i=0; i<nLibraryFiles; i++){
      args[j]=librarylist[i];
      j++;
   }
   args[j]=NULL;
   /*exec the linking build command at new process*/
   pid_t forkrank;
   forkrank=fork();
   if(forkrank == 0 ){
      //printf("child process is executing\n");
      print_array(args,j);
      exit(execvp(args[0],args));
   }else if(forkrank != -1){
      wait(NULL);
      //printf("parent waiting...\n");
   }else{
      perror("fork error\n");
   }


   free(compilerFlagsBuffer);
   free(linkerFlagsBuffer);
   free(sourcelistBuffer);
   fclose(pCompilerFlagsFile);
   fclose(pLinkerFlagsFile);
   fclose(pSourceListFile);
   return 0; 
}