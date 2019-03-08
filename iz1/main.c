
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

void printIntArr(int *arr, size_t n);
void swap(int *a, int *b);
void findMaxs(int *arr, size_t n, size_t m, int *maxArr);

/**
 * Пишет в стандартный вывод элементы массива arr
 * @param arr
 * @param n
 */
void printIntArr(int *arr, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void swap(int *a, int *b)
{
    int t = *b;
    *b = *a;
    *a = t;
}

/**
 * Ищет m наибольших элементов в массиве arr и записывает их в массив maxArr
 * @param arr
 * @param n
 * @param m
 * @param maxArr
 * @return указатель на массива maxArr
 */
void findMaxs(int *arr, size_t n, size_t m, int *maxArr)
{
    int maxValue;
    int i, j;

    for(i = 0; i < m; i++)
    {
        maxValue = arr[i];

        for(j = i+1; j < n; j++)
        {
            if(arr[j] > maxValue)
            {
                maxValue = arr[j];
                swap(&arr[i], &arr[j]);
            }
        }
        maxArr[i] = maxValue;
    }
}

int readNum(size_t * num){
    return scanf("%zd", num);
}

int readArr(int * arr, size_t n){
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &arr[i]) != 1)
        {
            return -1;
        }
    }
    return 0;
}

int runMaxSearch(){
    size_t n, m;
    int *arr;
    int *maxArr;

    if (readNum(&n) == -1){
        return -1;
    };

    arr = calloc(n, sizeof(int));
    if (arr == NULL)
    {
        return -1;
    }
    if (readArr(arr, n) == -1){
        free(arr);
        return -1;
    };

    if (readNum(&m) == -1){
        return 0;
    };

    if (m > n)
    {
        free(arr);
        return -1;
    }

    maxArr = malloc(sizeof(*maxArr) * m);
    if (maxArr == NULL)
    {
        free(arr);
        return -1;
    }

    findMaxs(arr, n, m, maxArr);
    printIntArr(maxArr, m);

    free(maxArr);
    free(arr);

    return 0;
}

int main(void)
{
    int errCode = runMaxSearch();

    if (errCode == -1){
        printf("[error]");
    }

    return 0;
}
