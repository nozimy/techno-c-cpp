
/**
 * Студент группы АПО-13 Юнусов Нозимжон
 * */

/**
 * Задача A-1. Задача о поиске максимума
 *
 * Составить программу поиска M наибольших чисел в заданном массиве целых (типа int) чисел A длины N.
 * Числа M, N и элементы массива A подаются на стандартный ввод программы в следующем порядке:
 * N
 * A1 A2 ... AN
 * M
 *
 * Процедура поиска должна быть оформлена в виде отдельной функции, которой подается на вход массив
 * целых чисел, выделенный в динамической памяти, его длина и требуемое количество максимальных чисел.
 * На выход функция должна возвращать динамически выделенный массив с найденными значениями.
 *
 * Программа должна уметь обрабатывать ошибки во входных данных. В случае возникновения ошибки
 * нужно вывести в поток стандартного вывода сообщение "[error]" и завершить выполнение программы.
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void print_int_arr(const int *arrayToPrint, size_t arraySize);

int swap_array_elems(int *i, int *j);

int* find_k_max(int *sourceArray, size_t sourceArraySize, size_t k);

int run_max_search();

int stdin_read_num(size_t *int_number);

int stdin_read_arr(int *arrayToStore, size_t arraySize);

int free_get_error(int *array);


static const int ERROR_CODE = -1;

int main(void)
{
    int errCode = run_max_search();

    if (errCode == ERROR_CODE){
        printf("[error]");
    }

    return 0;
}

/**
 * Пишет в стандартный вывод элементы массива arr
 * @param arrayToPrint
 * @param arraySize
 */
void print_int_arr(const int *arrayToPrint, size_t arraySize)
{
    if (arrayToPrint != NULL){
        for (int i = 0; i < arraySize; i++)
        {
            printf("%d ", arrayToPrint[i]);
        }
        printf("\n");
    }
}

/**
 * меняет местами значения указателей i и j между собой
 * */
int swap_array_elems(int *i, int *j)
{
    if (i != NULL && j != NULL){
        int t = *j;
        *j = *i;
        *i = t;
        return 0;
    } else {
        return ERROR_CODE;
    }
}

/**
 * Аллоцирует массив kMaxArray размером k,
 * ищет k наибольших элементов в массиве sourceArray и
 * записывает их в массив kMaxArray
 *
 * @param sourceArray
 * @param sourceArraySize
 * @param k
 * @param kMaxArray
 * @return указатель на массив kMaxArray
 */
int* find_k_max(int *sourceArray, size_t sourceArraySize, size_t k)
{
    int *kMaxArray = malloc(sizeof(*kMaxArray) * k);
    if (kMaxArray == NULL || sourceArray == NULL)
    {
        return NULL;
    }

    for(int i = 0; i < k; i++)
    {
        int maxValue = sourceArray[i];

        for(int j = i+1; j < sourceArraySize; j++)
        {
            if(sourceArray[j] > maxValue)
            {
                maxValue = sourceArray[j];
                swap_array_elems(&sourceArray[i], &sourceArray[j]);
            }
        }
        kMaxArray[i] = maxValue;
    }

    return kMaxArray;
}

int stdin_read_num(size_t *int_number){
    if (int_number != NULL) {
        return scanf("%zd", int_number);
    }
    return ERROR_CODE;

}

int stdin_read_arr(int *arrayToStore, size_t arraySize){
    if (arrayToStore == NULL){
        return ERROR_CODE;
    }
    for (int i = 0; i < arraySize; i++)
    {
        if (scanf("%d", &arrayToStore[i]) != 1)
        {
            return ERROR_CODE;
        }
    }
    return 0;
}


int free_get_error(int *array){
    if (array != NULL) free(array);
    return ERROR_CODE;
}

int run_max_search(){
    size_t n, m = 0;
    int *arr = NULL;
    int *maxArr = NULL;

    if (stdin_read_num(&n) == ERROR_CODE){
        return ERROR_CODE;
    };

    arr = calloc(n, sizeof(int));
    if (arr == NULL)
    {
        return ERROR_CODE;
    }
    if (stdin_read_arr(arr, n) == ERROR_CODE){
        return free_get_error(arr);
    };

    if (stdin_read_num(&m) == ERROR_CODE){
        return free_get_error(arr);
    };

    if (m > n)
    {
        return free_get_error(arr);
    }


    maxArr = find_k_max(arr, n, m);
    if (maxArr == NULL){
        return free_get_error(arr);
    }
    print_int_arr(maxArr, m);

    free(maxArr);
    free(arr);

    return 0;
}

