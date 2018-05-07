
// make compiller happy so you could use strptime function
#define __USE_XOPEN
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <math.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>




#define NOOPENFD 1024
const char dateFormat[] = "%Y-%m-%d %H:%M:%S";

struct parsedArgs {
    char* path;
    char direction;
    char* date;
};

struct parsedArgs stupidGlobalVariableButStackSaysYouCannotDoItOtherWay;
#define SGVBSSYCDIOW stupidGlobalVariableButStackSaysYouCannotDoItOtherWay

struct parsedArgs parseArgs (int argc, char* argv[]) {
    if(argc < 3)
        exit(EXIT_FAILURE);
    struct parsedArgs args;

    args.path = argv[1];
    args.direction = *argv[2];
    args.date = argv[3];

    //printf("%s %c %s\n", args.path, args.direction, args.date);


    return args;
}

char* getPermissions(const struct stat* stat_buffer) {

    char* permissions = (char*) malloc(sizeof(char) * 11);
    permissions[0] = S_ISDIR(stat_buffer -> st_mode) ? 'd' : '-';
    permissions[1] = (stat_buffer -> st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (stat_buffer -> st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (stat_buffer -> st_mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (stat_buffer -> st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (stat_buffer -> st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (stat_buffer -> st_mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (stat_buffer -> st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (stat_buffer -> st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (stat_buffer -> st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    return permissions;
}

int compareDate (char* date, char direction, time_t fileTime) {

    struct tm* parsedDate = (struct tm*) malloc(sizeof(struct tm));

    if(strptime(date, dateFormat, parsedDate) == NULL)
        exit(EXIT_FAILURE);

    time_t parsedTime = mktime(parsedDate);

    if(direction == '='){
        //printf("%d %d\n", parsedTime, fileTime);
        //if (fabs(difftime(parsedTime, fileTime)) > 0) printf(" %c", 8); //Bez tego nie działa i nie wiem dlaczego
        return fabs(difftime(parsedTime, fileTime)) < 0.1 ? 1 : 0;
    } else if(direction == '>'){
        return difftime(parsedTime, fileTime) > 0 ? 1 : 0;
    } else if(direction == '<'){
        return difftime(parsedTime, fileTime) < 0 ? 1 : 0;
    } else
        exit(EXIT_FAILURE);
}

void printFileInfo(const struct stat* statBuffer,const char* absPath) {

    char* permissions =  (char*) getPermissions(statBuffer);

    printf("%-80s%-12lld%-8s      %-16s",
           absPath,
           (long long int) statBuffer->st_size,
           permissions,
           ctime(&(statBuffer->st_mtime))
    );
    free(permissions);
}


void processFileStructure( char* path,
                           char direction,
                           char* date) {

           DIR* handle = opendir(path);
           struct dirent* currentDir = readdir(handle);

           while(currentDir != NULL) {

               if (strcmp(currentDir -> d_name, ".") == 0 || strcmp(currentDir -> d_name, "..") == 0) {
                   currentDir = readdir(handle);
               } else {

               char* currentDirPath = (char*) calloc(PATH_MAX, sizeof(char));
               strcat(strcat(strcat(currentDirPath, path),"/"), currentDir->d_name);

               if(currentDir -> d_type == DT_DIR) {

                    if(fork() == 0) {
                        processFileStructure(currentDirPath, direction, date);
                        exit(EXIT_SUCCESS);
                    }


               }
               else if(currentDir -> d_type == DT_REG) {


                    struct stat* statBuffer = (struct stat*) malloc(sizeof(struct stat));


                    if(stat(currentDirPath, statBuffer) != -1)
                    if(compareDate(date, direction, statBuffer -> st_mtime)) {
                       printFileInfo(statBuffer, currentDirPath);

                   }
               }

               currentDir = readdir(handle);

               }
       }
       errno = 0;
       while (errno != ECHILD)
          wait(NULL);
 }

 int fn (const char *fpath, const struct stat *sb,
            int typeflag, struct FTW *ftwbuf) {

                if (typeflag == FTW_F && compareDate(SGVBSSYCDIOW.date, SGVBSSYCDIOW.direction, sb -> st_mtime))
                       printFileInfo(sb, fpath);


                    return 0;
            }


int main( int argc, char* argv[] ) {

    struct parsedArgs args = parseArgs(argc, argv);


    if(!(args.direction == '=' || args.direction == '>' || args.direction == '<')) {
        printf("Wrong second argument \n");
        return EXIT_FAILURE;
    }

/*
    struct rlimit* limit = (struct rlimit*) malloc (sizeof(struct rlimit));
    getrlimit(RLIMIT_NPROC, limit);
    printf("%li\n%li\n", limit -> rlim_cur, limit -> rlim_max);
*/

    #ifdef NFTW
        SGVBSSYCDIOW = parseArgs(argc, argv);
        nftw(SGVBSSYCDIOW.path, fn, NOOPENFD, FTW_PHYS);
    #else
        processFileStructure(args.path, args.direction, args.date);

    #endif

    return 0;
}
