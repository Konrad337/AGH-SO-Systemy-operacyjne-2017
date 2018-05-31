#include <math.h>
#include <limits.h>
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
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <math.h>

void clean() {

}
#define CEIL(x, y) (((x) + (y) - 1) / (y))
#define MAX(X, Y) (X > Y ? X : Y)
#define MIN(X, Y) (X < Y ? X : Y)

struct picture_info {
    int ** picture;
    float ** filter;
    int ** filtered_pic;
    int filter_size;
    int width;
    int height;
    int range;
    int done;
    int thread_total;


};
struct picture_info img;

void *thread_handler(void *args) {

    int thread_num = *(int *) args;
    int num_of_threads = img. thread_total;
    //printf("%i\n", thread_num);

    for (int x = thread_num * img. width / num_of_threads; x < (thread_num + 1) * img. width / num_of_threads; x++) {
        for (int y = 0; y < img. height; y++) {
            double s = 0;

            for (int i = 0; i < img. filter_size; i++) {
                for (int j = 0; j < img. filter_size; j++) {
                    int res_x = MIN(img. width - 1, MAX(0, x - CEIL(img. filter_size, 2) + i));
                    int res_y = MIN(img. height - 1, MAX(0, y - CEIL(img. filter_size, 2) + j));
                    s += img. picture[res_y][res_x] * img. filter[i][j];
                }
            }
            img. filtered_pic[y][x] = (int) round(s);
        }
    }
    img. done++;
    //printf("%i\n", thread_num);
    //printf("%i t done\n", img->done);
}

void finish(int signal) {

    printf("%c%cCleaning and exiting \n", 8, 8);
    clean();
    exit(EXIT_SUCCESS);

}

int main( int argc, char* argv[] ) {

    struct timespec cur_time;

    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigaction(SIGINT, &grand_finale, NULL);

    if(argc < 5)
        exit(EXIT_FAILURE);
    int threads_n =  strtol(argv[1], NULL, 10);
    char* pic_path = argv[2];
    char* filter_path = argv[3];
    char* out_path = argv[4];
    struct picture_info pic;

    FILE *file = fopen(pic_path, "r");
    if (file == NULL) {
        perror("Error while opening file");
    }

    fscanf(file, "P2\n");
    fscanf(file, "%d*[ \n]", &pic.width);
    fscanf(file, "%d*[ \n]", &pic.height);
    fscanf(file, "%d*[ \n]", &pic.range);

    pic.picture = (int**) malloc(sizeof(int*) * pic.height);
    pic.filtered_pic = (int**) malloc(sizeof(int*) * pic.height);

    for(int i = 0; i < pic.height; i++) {
        pic.picture[i] = (int*) malloc(sizeof(int) * pic.width);
        pic.filtered_pic[i] = (int*) malloc(sizeof(int) * pic.width);
    }

    for(int i = 0; i < pic.height; i++) {
        for(int j = 0; j < pic.width; j++) {
            fscanf(file, "%d*[ \n]", &pic.picture[i][j]);
            //printf("%i ", pic.picture[i][j]);
        }
        //printf("\n");
    }
    fclose(file);
    file = fopen(filter_path, "r");
    fscanf(file, "%d*[ \n]", &pic.filter_size);

    pic.filter = (float**) malloc(sizeof(float*) * pic.filter_size);
    for(int i = 0; i < pic.filter_size; i++) {
        pic.filter[i] = (float*) malloc(sizeof(float) * pic.filter_size);
    }

    for(int i = 0; i < pic.filter_size; i++) {
        for(int j = 0; j < pic.filter_size; j++) {
            fscanf(file, "%f[ \n]", &pic.filter[i][j]);
            //printf("%f ", pic.filter[i][j]);
        }
        //printf("\n");
    }
    fclose(file);
    pic.thread_total = threads_n;
    pic.done = 0;
    img = pic;

    struct timeval start, uStart, sStart;
    struct timeval end, uEnd, sEnd;
    struct rusage usage;


    gettimeofday(&start, 0);
    getrusage(RUSAGE_SELF, &usage);
    uStart = usage.ru_utime;
    sStart = usage.ru_stime;

    pthread_t* threads_id = (pthread_t*) malloc(sizeof(pthread_t) * threads_n);
    for(int i = 0; i < threads_n ; i++) {
        int* thread_number = (int*) malloc(sizeof(int));
        *thread_number = i;
        pthread_create(&threads_id[i], NULL, thread_handler, thread_number);
    }

    for (int i = 0; i < threads_n; i++) {
        void *nothing;
        pthread_join(threads_id[i], &nothing);
    }

    gettimeofday(&end, 0);
    getrusage(RUSAGE_SELF, &usage);
    uEnd = usage.ru_utime;
    sEnd = usage.ru_stime;

    long real = (end.tv_sec-start.tv_sec)*1000000 + end.tv_usec-start.tv_usec;
    long user = (uEnd.tv_sec-uStart.tv_sec)*1000000 + uEnd.tv_usec-uStart.tv_usec;
    long systemT = (sEnd.tv_sec-sStart.tv_sec)*1000000 + sEnd.tv_usec-sStart.tv_usec;

    printf(" Real time: %li milisecs\n"
           " User time: %li milisecs\n"
           " System time: %li milisecs\n"
           "%s %i\n \n",
           real,
           user,
           systemT,
          argv[0],
          threads_n);


    file = fopen(out_path, "w");
    fprintf(file, "P2\n");
    fprintf(file, "%i %i\n", pic.width, pic.height);
    for(int i = 0; i < pic.height; i++) {
        for(int j = 0; j < pic.width; j++) {
            fprintf(file, "%i ", pic.filtered_pic[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);


    clean();
    return 0;
}
