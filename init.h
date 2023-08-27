#ifndef INIT_H_   /* Include guard */
#define INIT_H_

unsigned char* readTrack(char* fileName, int* w, int* h);
PIXEL_LINK** initTrack(unsigned char* pixelData, int width, int height);
PIXEL_LINK* sortLinkedLists(PIXEL_LINK** linkHeads);


#endif
