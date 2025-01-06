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
    int counter = digit_counter(argv[1]);
    printf("The number of digits in the string is: %d\n",counter);
}