#include "stdio.h"

int FindBinary(int* array, int size, int key) {
  int left = -1;
  int right = size;

  while (left <= right) {
    int middle = (left + right) / 2;

    if (array[middle] < key) {
      left = middle + 1;
    }
    else if (array[middle] > key) {
      right = middle - 1;
    }
    else {
      return middle;
    }
  }

  return -1;
}

int main() {
  int key = 0;
  scanf("%d", &key);

  int array[] = {1, 2, 10, 12, 14};

  int idx = FindBinary(array, sizeof(array) / sizeof(*array), key);
  if (idx != -1) {
    printf("Found %d at %d place\n", key, idx);
  }
  else {
    printf("Can't find key %d in the given array\n", key);
  }

  return 0;
}