#include <check.h>
#include <stdlib.h>

#include "matrix.h"

START_TEST(test_one_element_matrix)
{
    matrix_t matrix = { NULL, 1, 1};
    
    m_allocate(&matrix);

    matrix.element[0][0] = 5;
    
    int column = 0;
    int min = m_find_min(&matrix, &column);
    
	m_free(&matrix);
	
    ck_assert_int_eq(column, 0);
    ck_assert_int_eq(min, 5);
}
END_TEST

START_TEST(test_vertical_vector)
{
    matrix_t matrix = { NULL, 5, 1};
    
    m_allocate(&matrix);
    
    for (size_t i = 0; i < matrix.rows; i++)
        for (size_t j = 0; j < matrix.columns; j++)
			matrix.element[i][j] = i + j;
		
	matrix.element[3][0] = -3;
    
    int column = 0;
    int min = m_find_min(&matrix, &column);
    
	m_free(&matrix);
	
    ck_assert_int_eq(column, 0);
    ck_assert_int_eq(min, -3);
}
END_TEST

START_TEST(test_horizontal_vector)
{
    matrix_t matrix = { NULL, 1, 5};
    
    m_allocate(&matrix);
    
    for (size_t i = 0; i < matrix.rows; i++)
        for (size_t j = 0; j < matrix.columns; j++)
			matrix.element[i][j] = i * j;
		
	matrix.element[0][2] = -9;
    
    int column = 0;
    int min = m_find_min(&matrix, &column);
    
	m_free(&matrix);
	
    ck_assert_int_eq(column, 2);
    ck_assert_int_eq(min, -9);
}
END_TEST

START_TEST(test_square_matrix)
{
    matrix_t matrix = { NULL, 3, 3};
    
    m_allocate(&matrix);
    
    for (size_t i = 0; i < matrix.rows; i++)
        for (size_t j = 0; j < matrix.columns; j++)
			matrix.element[i][j] = i + j * 2;
		
	matrix.element[0][1] = -1;
    
    int column = 0;
    int min = m_find_min(&matrix, &column);
    
	m_free(&matrix);
	
    ck_assert_int_eq(column, 1);
    ck_assert_int_eq(min, -1);
}
END_TEST

START_TEST(test_similar_numbers)
{
    matrix_t matrix = { NULL, 4, 6};
    
    m_allocate(&matrix);
    
    for (size_t i = 0; i < matrix.rows; i++)
        for (size_t j = 0; j < matrix.columns; j++)
			matrix.element[i][j] = 5;
    
    int column = 0;
    int min = m_find_min(&matrix, &column);
    
	m_free(&matrix);
	
    ck_assert_int_eq(column, 0);
    ck_assert_int_eq(min, 5);
}
END_TEST

Suite *m_find_min_suite(void)
{
    Suite *s = suite_create("test_m_find_min");
    
    TCase *tc = tcase_create("all");
    
    tcase_add_test(tc, test_one_element_matrix);
	tcase_add_test(tc, test_vertical_vector);
	tcase_add_test(tc, test_horizontal_vector);
	tcase_add_test(tc, test_square_matrix);
	tcase_add_test(tc, test_similar_numbers);
	
	suite_add_tcase(s, tc);
    
    return s;
}
