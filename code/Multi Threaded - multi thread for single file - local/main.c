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

        // Load image
        int statusOpeningImage = OpenImage(inputFile, &header, &body, &bodySize, &width, &height);  
        if (statusOpeningImage != OPENING_IMAGE_SUCCESS) {
            printf("Error: Failed to open image.\n");
            return statusOpeningImage;
        }

        int numChunks = 8; // Number of chunks
        size_t chunkSize = bodySize / numChunks;

        // Allocate arrays for chunks
        unsigned char* chunks[numChunks];
        for (int i = 0; i < numChunks; i++) {
            size_t currentChunkSize = (i == numChunks - 1) ? bodySize - (i * chunkSize) : chunkSize;
            chunks[i] = (unsigned char*)malloc(currentChunkSize);
            memcpy(chunks[i], body + (i * chunkSize), currentChunkSize);
        }

        // Process chunks in parallel
        pthread_t threads[numChunks];
        GrayScaleTask tasks[numChunks];
        for (int i = 0; i < numChunks; i++) {
            size_t currentChunkSize = (i == numChunks - 1) ? bodySize - (i * chunkSize) : chunkSize;

            tasks[i].grayValues = chunks[i];
            tasks[i].start = 0; // Start of the chunk array
            tasks[i].end = currentChunkSize; // End of the chunk array

            pthread_create(&threads[i], NULL, ChunkToGrayscale, &tasks[i]);
        }

        // Wait for all threads to finish
        for (int i = 0; i < numChunks; i++) {
            pthread_join(threads[i], NULL);
        }

        // Merge processed chunks into a single array
        unsigned char* processedBody = (unsigned char*)malloc(bodySize);
        for (int i = 0; i < numChunks; i++) {
            size_t currentChunkSize = (i == numChunks - 1) ? bodySize - (i * chunkSize) : chunkSize;
            memcpy(processedBody + (i * chunkSize), chunks[i], currentChunkSize);
        }

        // Save the merged result
        sprintf(outputFile, OUTPUT_FILE_NAME, index);
        WriteImage(outputFile, header, processedBody, bodySize);

        // Free resources
        free(header);
        free(body);
        free(processedBody);
    }


    return 0;
}
