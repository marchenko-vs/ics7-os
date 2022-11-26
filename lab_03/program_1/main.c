#include <stdio.h>
#include <stdlib.h>

#define MIN_SIZE 1
#define MAX_SIZE 10

#define SUCCESS 0
#define INCORRECT_DATA_TYPE -1
#define INCORRECT_SIZE -2

#define NOT_SQUARE_MATRIX -3

int create_matrix(int *rows, int *cols)
{
    if (scanf("%d", rows) != 1)
        return INCORRECT_DATA_TYPE;

    if (MIN_SIZE > *rows || MAX_SIZE < *rows)
        return INCORRECT_SIZE;

    if (scanf("%d", cols) != 1)
        return INCORRECT_DATA_TYPE;

    if (MIN_SIZE > *cols || MAX_SIZE < *cols)
        return INCORRECT_SIZE;

    if (*rows != *cols)
        return NOT_SQUARE_MATRIX;

    return SUCCESS;
}

void spiral_matrix(int (*matrix)[MAX_SIZE], const int size)
{
    int counter = 0, padding = 0, i = 0, j = 0;

    while (padding < size)
    {
        while (j < size - padding)
        {
            matrix[i][j] = ++counter;
            j++;
        }

        i++;
        j--;

        while (i < size - padding)
        {
            matrix[i][j] = ++counter;
            i++;
        }

        i--;
        j--;

        while (j >= padding)
        {
            matrix[i][j] = ++counter;
            j--;
        }

        i--;
        j++;

        while (i > padding)
        {
            matrix[i][j] = ++counter;
            i--;
        }

        i++;
        j++;
        padding++;
    }
}

void print_matrix(int (*const matrix)[MAX_SIZE], const int rows,
const int cols)
{
    for (int i = 0; rows > i; i++)
    {
        for (int j = 0; cols > j; j++)
            printf("%d ", matrix[i][j]);

        printf("\n");
    }
}

int main(void)
{
    int user_rows, user_cols, result;

    printf("Enter sizes of matrix:\n");

    if ((result = create_matrix(&user_rows, &user_cols)) ==
        INCORRECT_SIZE)
        return INCORRECT_SIZE;

    if (result == INCORRECT_DATA_TYPE)
        return INCORRECT_DATA_TYPE;

    if (result == NOT_SQUARE_MATRIX)
        return NOT_SQUARE_MATRIX;

    int user_matrix[MAX_SIZE][MAX_SIZE];

    spiral_matrix(user_matrix, user_rows);
    printf("Result matrix:\n");
    print_matrix(user_matrix, user_rows, user_cols);

    return EXIT_SUCCESS;
}
