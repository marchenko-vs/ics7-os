#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <stdlib.h>

#define ERR_MEMORY_ALLOC 1
#define ERR_DATA_TYPE 2
#define ERR_MATRIX_SIZE 3

typedef struct
{
    int **element;
    int rows;
    int columns;
} matrix_t;

void m_free(matrix_t *matrix);
int m_allocate(matrix_t *matrix);
int m_get_size(matrix_t *matrix);
int m_scan(matrix_t *matrix);
void m_copy_size(matrix_t *matrix, int rows, int columns);
void m_copy(matrix_t *matrix_1, matrix_t *matrix_2);
void m_print(matrix_t *matrix);
void ma_print(matrix_t *array, size_t size); // ma - array of matrices
int m_find_min(matrix_t *matrix, int *column);
int m_find_max(matrix_t *matrix, int *column);
void m_process(matrix_t *matrix);

#endif // _MATRIX_H_
