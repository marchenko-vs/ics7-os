#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"

void m_free(matrix_t *matrix)
{
    free(matrix->element[0]);
    free(matrix->element);
}

int m_allocate(matrix_t *matrix)
{
    int **ptrs = calloc(matrix->rows, sizeof(int *));

    if (!ptrs)
        return ERR_MEMORY_ALLOC;

    int *data = calloc(matrix->columns * matrix->rows, sizeof(int));

    if (!data)
    {
        free(ptrs);

        return ERR_MEMORY_ALLOC;
    }

    matrix->element = ptrs;

    for (size_t i = 0; i < matrix->rows; i++)
        matrix->element[i] = data + i * matrix->columns;

    return EXIT_SUCCESS;
}

int m_get_size(matrix_t *matrix)
{
    printf("Enter size of the matrix: ");

    if (scanf("%d", &matrix->rows) != 1)
        return ERR_DATA_TYPE;

    if (matrix->rows < 1)
        return ERR_MATRIX_SIZE;

    if (scanf("%d", &matrix->columns) != 1)
        return ERR_DATA_TYPE;

    if (matrix->columns < 1)
        return ERR_MATRIX_SIZE;

    return EXIT_SUCCESS;
}

int m_scan(matrix_t *matrix)
{
    printf("Enter elemets of the matrix:\n");

    for (size_t i = 0; i < matrix->rows; i++)
        for (size_t j = 0; j < matrix->columns; j++)
            if (scanf("%d", &matrix->element[i][j]) != 1)
                return ERR_DATA_TYPE;

    return EXIT_SUCCESS;
}

void m_copy_size(matrix_t *matrix, int rows, int columns)
{
    matrix->rows = rows;
    matrix->columns = columns;
}

void m_copy(matrix_t *matrix_1, matrix_t *matrix_2)
{
    for (size_t i = 0; i < matrix_2->rows; i++)
        for (size_t j = 0; j < matrix_2->columns; j++)
            matrix_2->element[i][j] = matrix_1->element[i][j];
}

void m_print(matrix_t *matrix)
{
    for (size_t i = 0; i < matrix->rows; i++)
    {
        for (size_t j = 0; j < matrix->columns; j++)
            printf("%d ", matrix->element[i][j]);

        printf("\n");
    }
}

void ma_print(matrix_t *array, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("Matrix number %zu\n", i + 1);
        m_print(&array[i]);
    }
}

void swap_elements(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int m_find_min(matrix_t *matrix, int *column)
{
    int min = matrix->element[0][0];
    *column = 0;

    for (size_t i = 0; i < matrix->rows; i++)
        for (size_t j = 0; j < matrix->columns; j++)
            if (matrix->element[i][j] < min)
            {
                min = matrix->element[i][j];
                *column = j;
            }

    return min;
}

int m_find_max(matrix_t *matrix, int *column)
{
    int max = matrix->element[0][0];
    *column = 0;

    for (size_t i = 0; i < matrix->rows; i++)
        for (size_t j = 0; j < matrix->columns; j++)
            if (matrix->element[i][j] > max)
            {
                max = matrix->element[i][j];
                *column = j;
            }

    return max;
}

void m_process(matrix_t *matrix)
{
    if (matrix->columns < 2)
        return;

    int column_min, column_max;

    m_find_min(matrix, &column_min);
    m_find_max(matrix, &column_max);

    if (column_min == column_max)
        return;

    for (size_t i = 0; i < matrix->rows; i++)
        swap_elements(&(matrix->element[i][column_min]), 
            &(matrix->element[i][column_max]));
}
