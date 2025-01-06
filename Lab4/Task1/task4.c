#include <stdio.h>

int digit_counter(const char* str) {
    int count = 0;
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            count++;
        }
        str++;
    }
    return count;
}

int main(int argc, char* argv[]) {
     if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }
    int counter = digit_counter(argv[1]);
    printf("digits exists: %d\n",counter);
}