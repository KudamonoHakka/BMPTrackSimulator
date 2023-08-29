#ifndef INIT_H_   /* Include guard */
#define INIT_H_

#define M_PI 3.14159265358979323846
unsigned char* readTrack(char* fileName, int* w, int* h);
PIXEL_LINK** initTrack(unsigned char* pixelData, int width, int height);
PIXEL_LINK* sortLinkedLists(PIXEL_LINK** linkHeads);
void cleanup(PIXEL_LINK* sortedHead);



#endif
