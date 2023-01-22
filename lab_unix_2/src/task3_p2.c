#include <stdio.h>
#include <stdlib.h>

#define NMAX 1024
#define MYMAX 10

#define ERROR_INVALID_LEN 1
#define ERROR_INVALID_NUMBER 2
#define ERROR_NOT_ENOUGH_NUMBERS 3

int input(int *p_b, int *p_e)
{
    int rc;
    while (p_b < p_e)
        if ((rc = scanf("%d", p_b)) != 1)
        {
            if (rc == EOF)
                return ERROR_NOT_ENOUGH_NUMBERS;
            return ERROR_INVALID_NUMBER;
        }
        else
            p_b++;
    return EXIT_SUCCESS;
}

void output(int *p_b, int *p_e)
{
    for (int *p_cur = p_b; p_cur < p_e; p_cur++)
        printf("%d ", *p_cur);
}

int count_unique(int *p_b, int *p_e)
{
    int count = 0;
    for (int *p_cur = p_b; p_cur < p_e; p_cur++)
    {
        int flag = 1;
        for (int *p_tmp = p_b; p_tmp < p_cur && flag == 1; p_tmp++)
            if (*p_tmp == *p_cur)
                flag = 0;
        count += flag;
    }

    return count;
}

int main(void)
{
    setbuf(stdout, NULL);
    int arr[NMAX] = {1, 1, 2, 3, 1, 4, 5, 5, 6, 7};
    size_t len = 10;

    printf("Count of unique elements = %d", count_unique(arr, arr + len));
    printf("\n");

    return EXIT_SUCCESS;
}
