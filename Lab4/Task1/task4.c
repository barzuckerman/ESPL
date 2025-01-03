#include <stdio.h>


int digit_counter(char* str);
int main(int argc, char **argv)
{
    digit_counter(argv[1]); 
    return 0;
}

int digit_counter(char* str)
{
    int count = 0;
    while (*str != '\0')
    {
        if (*str >= '0' && *str <= '9')
        {
            count++;
        }
        str++;
    }
    return count;
}