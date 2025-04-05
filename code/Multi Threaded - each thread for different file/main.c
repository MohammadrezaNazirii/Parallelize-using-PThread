#include "utilities.h"
#include <pthread.h>
#define INPUT_FILE_NAME "input/in%d.bmp"
#define OUTPUT_FILE_NAME "output/out%d.bmp"
#define NUM_THREADS 1

void * ImageThreadProcess(void* index){
    int bodySize;
	int width, height;

	double time_spent;
    double cummulativetime = 0;

    unsigned char *header = NULL;
	unsigned char *body = NULL;
	unsigned char *results = NULL;

	clock_t start, end;
    ImageData *imageData = (ImageData *) malloc(NUMBER_OF_INPUTS * sizeof(ImageData));
    
    char inputFile[15];
    char outputFilename[20];
    
    
    sprintf(inputFile, INPUT_FILE_NAME, (int)index);
    int statusOpeningImage = OpenImage(inputFile, &header, &body, &bodySize, &width, &height);
	if (statusOpeningImage != OPENING_IMAGE_SUCCESS) {
		printf("Error: An earlier error occured with the given description.\n");
		return statusOpeningImage;
	}
        
    imageData[(int)index - 1].grayValues = body;
    imageData[(int)index - 1].size = bodySize;
        
    start = clock();
        
    ConvertRGBtoGrayscale((void *) &imageData[(int)index - 1]);
    end = clock();


    time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time spent on input %d: %f microseconds \n", index, time_spent * 1000);

    cummulativetime += time_spent;
        
    sprintf(outputFilename, OUTPUT_FILE_NAME, index);
    WriteImage(outputFilename, header, body, bodySize);
    free(results);
	free(header);
	free(body);
    pthread_exit(NULL);

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
        for (int i =(8/NUM_THREADS)*t ; i< ((8/NUM_THREADS)*t)+(8/NUM_THREADS);i++){

            printf("Creating thread %d\n", t);
            rc = pthread_create(&thread[t], &attr,ImageThreadProcess, i+1);
            if (rc){
                printf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
			}
        }
        
    }

    pthread_attr_destroy(&attr);

    for(t=0; t<NUM_THREADS; t++){
        rc = pthread_join(thread[t], &status);
        if (rc){
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
        printf("Completed join with thread %d status=%ld\n",t, (long)status);
        pthread_exit(NULL);
    }

    return 0;
}
