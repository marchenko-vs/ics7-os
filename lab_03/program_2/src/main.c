#include <stdlib.h>
#include <stdio.h>

#include "matrix.h"

#define ERR_ARRAY_SIZE 1

int main(void)
{
    size_t array_size = 0;
	int tmp = 0;

    printf("Enter number of matrices: ");

    if (scanf("%d", &tmp) != 1)
	{
		printf("Error: incorrect data type.\n");
        return ERR_DATA_TYPE;
	}

    if (tmp < 1)
	{
		printf("Error: size of an array must be positive.\n");
		
        return ERR_ARRAY_SIZE;
	}
	
	array_size = tmp * 2;

    matrix_t *array = calloc(array_size, sizeof(matrix_t));

    if (!array)
	{
		printf("Error: memory can't be allocated.\n");
		
        return ERR_MEMORY_ALLOC;
	}

    int rc = EXIT_SUCCESS;
    size_t i = 0;

    for (i = 0; i < array_size; i += 2)
    {
        if (m_get_size(&array[i]))
        {
            rc = ERR_MATRIX_SIZE;
            break;
        }

        if (m_allocate(&array[i]))
        {
            rc = ERR_MEMORY_ALLOC;
            break;
        }

        if (m_scan(&array[i]))
        {
            rc = ERR_DATA_TYPE;
            break;
        }

        m_copy_size(&array[i + 1], array[i].rows, array[i].columns);

        if (m_allocate(&array[i + 1]))
        {
            rc = ERR_MEMORY_ALLOC;
            break;
        }

        m_copy(&array[i], &array[i + 1]);
    }

    if (rc == EXIT_SUCCESS)
    {
        for (size_t i = 0; i < array_size; i += 2)
            m_process(&array[i + 1]);
            
        printf("Result:\n");
        ma_print(array, array_size);

        for (size_t i = 0; i < array_size; i++)
            m_free(&array[i]);
    }
    else
    {
        for (size_t j = 0; j < i; j++)
            m_free(&array[j]);
    }

    free(array);

    return rc;
}
