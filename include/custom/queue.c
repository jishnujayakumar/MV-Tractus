#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "f_and_mv.h"
#define MAX_FRAMES 250

typedef struct frame_and_MV fmv;
fmv* queue = (fmv*) malloc( MAX_FRAMES * sizeof(fmv));
// fmv queue[MAX_FRAMES];
int front = 0;
int rear = -1;
int itemCount = 0;

fmv peek() {
   return queue[front];
}

bool isEmpty() {
   return itemCount == 0;
}

bool isFull() {
   return itemCount == MAX_FRAMES;
}

int size() {
   return itemCount;
}

void enqueueFrameData(fmv data) {

   if(!isFull()) {

      if(rear == MAX_FRAMES-1) {
         rear = -1;
      }

      queue[++rear] = data;
      itemCount++;
   }
}

fmv dequeueFrameData() {
   fmv data = queue[front++];

   if(front == MAX_FRAMES) {
      front = 0;
   }

   itemCount--;
   return data;
}
