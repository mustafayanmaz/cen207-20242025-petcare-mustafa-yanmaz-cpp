#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "methods.h"

// Yeni bir düğüm oluştur
MinHeapNode* newNode(char data, unsigned freq) {
    MinHeapNode* temp = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Min yığın oluştur
MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(minHeap->capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// İki düğümün yerini değiştir
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Min yığın düzenlemesi
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// En küçük düğümü çıkar
MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Düğümü yığına ekle
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Min yığın oluştur
MinHeap* buildMinHeap(char data[], int freq[], int size) {
    MinHeap* minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;

    for (int i = (minHeap->size - 2) / 2; i >= 0; --i)
        minHeapify(minHeap, i);

    return minHeap;
}

// Huffman ağacı oluştur
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    MinHeapNode* left, * right, * top;
    MinHeap* minHeap = buildMinHeap(data, freq, size);

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Kodları oluştur ve yazdır
void printCodes(MinHeapNode* root, int arr[], int top, char codes[256][MAX_TREE_HT]) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1, codes);
    }
    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1, codes);
    }
    if (!(root->left) && !(root->right)) {
        codes[(int)root->data][0] = '\0';
        printf("%c: ", root->data);
        for (int i = 0; i < top; ++i) {
            printf("%d", arr[i]);
            codes[(int)root->data][i] = '0' + arr[i];
            codes[(int)root->data][i + 1] = '\0';
        }
        printf("\n");
    }
}

// Huffman kodlarını oluştur
void HuffmanCodes(char data[], int freq[], int size, char codes[256][MAX_TREE_HT]) {
    MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;
    printCodes(root, arr, top, codes);
}

// Verilen metni sıkıştır
void compress(char* input, char codes[256][MAX_TREE_HT], char* output) {
    output[0] = '\0';
    for (int i = 0; input[i] != '\0'; ++i)
        strcat(output, codes[(int)input[i]]);
}

// Huffman ağacını kullanarak çöz
void decompress(MinHeapNode* root, char* compressed, char* output) {
    MinHeapNode* current = root;
    int j = 0;
    for (int i = 0; compressed[i] != '\0'; ++i) {
        current = (compressed[i] == '0') ? current->left : current->right;

        if (!(current->left) && !(current->right)) {
            output[j++] = current->data;
            current = root;
        }
    }
    output[j] = '\0';
}
