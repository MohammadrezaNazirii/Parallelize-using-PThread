#include "utilities.h"
#include <pthread.h>
#define INPUT_FILE_NAME "input/in%d.bmp"
#define OUTPUT_FILE_NAME "output/out%d.bmp"
#define NUM_THREADS 8

void * ImageThreadProcess(void* index){
    int bodySize;
	int width, height;

	double time_spent;
    double cummulativetime = 0;

    unsigned char *header = NULL;
	unsigned char *body = NULL;
	unsigned char *results = NULL;

    
    char inputFile[15];
    char outputFile[20];


   
    sprintf(inputFile, INPUT_FILE_NAME,(int)index);

    int statusOpeningImage = OpenImage(inputFile, &header, &body, &bodySize, &width, &height);
	if (statusOpeningImage != OPENING_IMAGE_SUCCESS) {
		printf("Error: An earlier error occured with the given description.\n");
		return statusOpeningImage;
	}
    int numThreads = 4; 
    pthread_t threads[numThreads];
    GrayScaleTask tasks[numThreads];

    size_t chunkSize = bodySize / numThreads;
    for (int i = 0; i < numThreads; i++) {
        tasks[i].grayValues = body;
        tasks[i].start = i * chunkSize;
        tasks[i].end = (i == numThreads - 1) ? bodySize : (i + 1) * chunkSize;
        pthread_create(&threads[i], NULL, ChunkToGrayscale, &tasks[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
        
    sprintf(outputFile, OUTPUT_FILE_NAME, ((int)index));
    WriteImage(outputFile, header, body, bodySize);
		
    free(header);
    free(body);

}
int main(int argc, char *argv[])
{
	
    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;

    int rc, t;
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
    for(t=0; t<NUM_THREADS; t++){
        printf("Creating thread %d\n", t);
        rc = pthread_create(&thread[t], &attr,ImageThreadProcess,t+1);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);

    for(t=0; t<NUM_THREADS; t++){
        rc = pthread_join(thread[t], &status);
        if (rc){
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Completed join with thread %d \n",t);
        pthread_exit(NULL);
    }


    return 0;
}
