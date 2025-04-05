#include "utilities.h"

#define INPUT_FILE_NAME "input/in%d.bmp"
#define OUTPUT_FILE_NAME "output/out%d.bmp"

int main(int argc, char *argv[])
{
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
    char outputFile[20];

    for (int index = 1; index <= NUMBER_OF_INPUTS; index++) {
        sprintf(inputFile, INPUT_FILE_NAME, index);

        int statusOpeningImage = OpenImage(inputFile, &header, &body, &bodySize, &width, &height);
	    if (statusOpeningImage != OPENING_IMAGE_SUCCESS) {
		    printf("Error: An earlier error occured with the given description.\n");
		    return statusOpeningImage;
	    }
        int numThreads = 8;
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
        
        sprintf(outputFile, OUTPUT_FILE_NAME, index);
        WriteImage(outputFile, header, body, bodySize);

        free(header);
        free(body);
    }


    return 0;
}
