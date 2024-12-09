#ifndef METHODS_H
#define METHODS_H

#define MAX_TREE_HT 100

typedef struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode* left, * right;
} MinHeapNode;

typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    MinHeapNode** array;
} MinHeap;

// Fonksiyon prototipleri
MinHeapNode* newNode(char data, unsigned freq);
MinHeap* createMinHeap(unsigned capacity);
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* minHeap, int idx);
MinHeapNode* extractMin(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode);
MinHeap* buildMinHeap(char data[], int freq[], int size);
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size);
void printCodes(MinHeapNode* root, int arr[], int top, char codes[256][MAX_TREE_HT]);
void HuffmanCodes(char data[], int freq[], int size, char codes[256][MAX_TREE_HT]);
void compress(char* input, char codes[256][MAX_TREE_HT], char* output);
void decompress(MinHeapNode* root, char* compressed, char* output);

#endif
