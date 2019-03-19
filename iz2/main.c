/**
 * Студент группы АПО-13 Юнусов Нозимжон
 * */
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/**
 * Задача B-5. Парсер, логические выражения.
 *
 * Требуется написать программу, которая способна вычислять логическе выражения.
 * Допустимые выражения чувствительны к регистру и могут содержать:
 *      1) знаки операций 'and' - и, 'or' - или, 'not' - отрицание, 'xor' - сложение по модулю 2
 *      2) Скобки '(', ')'
 *      3) Логические значения 'True', 'False'
 *      4) Пустое выражение имеет значение "False"
 *
 * Также требуется реализовать возможность задания переменных, которые могут состоять только
 * из маленьких букв латинского алфавита (к примеру, 'x=True').
 * Объявление переменной имеет формат:
 *      <имя>=<True|False>; // допустимы пробелы
 *
 * Допускается несколько объявлений одной и той же переменной, учитывается последнее.
 * Все переменные, используемые в выражении, должны быть заданы заранее. Также запрещены
 * имена, совпадающие с ключевыми словами (and, or и т.д.).
 *
 * Необходимо учитывать приоритеты операций. Пробелы ничего не значат.
 *
 * Если выражение или объявление не удовлетворяют описанным правилам, требуется вывести в
 * стандартный поток вывода "[error]" (без кавычек).
 *
 * Examples:
 *      Input:
 *          iscat=True;
 *          isdog=False;
 *          (not iscat and isdog) or True
 *      Output:
 *          True
 *
 */

/**
 * Описание решения
 *      Если логическое выражение: (iscat and not isdog)
 *          то "(", ")", "iscat", "and", "not", "isdog" - это токены
 *      строится дерево токенов с учетом приоритетов операций
 *      начиная с корневого элемента разрешается это дерево, результат - булево значение
 *      если есть переменные, то они хранятся в словаре переменных variablesDictionary
 *      также производится валидация каждой строки перед выполнением операций сохранения переменных
 *          или вычисления логического выражения
 */


/* Для хранения переменных iscat, isdog и т.д. */
typedef struct dict_t_struct {
    char *key;
    bool value;
    struct dict_t_struct *next;
} dict_t;

dict_t **dict_alloc(void);
void dict_dealloc(dict_t **dict);
int get_dict_item(dict_t *dict, char *key);
void del_dict_item(dict_t **dict, char *key);
void add_dict_item(dict_t **dict, char *key, bool value);
void add_var_to_dict(dict_t **dict, char *assignLine);

enum Operators {
    Or, Xor, And, Not, Brackets, ClosedBrackets, Identifier
};


typedef struct Node {
    enum Operators operator;
    bool value;
    struct Node *primary_child;
    struct Node *secondary_child;
} node_t;

int run_parser();
void remove_unnecessary_spaces(char *line, bool removeAll);
bool is_assignment_valid(char * line);
bool is_expression_valid(char * line);
bool parse_logical_expression(bool *isError, char *line, dict_t **dict);

void insert_token_node(node_t **root, node_t *newNode);
bool resolve_token_node(node_t *node);

int get_priority(int operator);
int get_operator_type(char *line, int firstIndex, int lastIndex);

bool is_low_letter(char character);
bool is_var_name(char *line, int firstIndex, int lastIndex);
bool is_true_str(char *line, int firstIndex);
bool is_false_str(char *line, int firstIndex);
int find_char_in_arr(char *line, char character);
bool is_word_braking_char(const char *line, int index);

bool increase_stack_memory(node_t ***stack, size_t *allocSize);

void free_if_not_null(char *line);
void free_run_parser(char *line, dict_t **variablesDictionary);
void free_token_node_tree(node_t *rootNode);

static const int ERROR = -1;

int main(void) {
    int errorCode = run_parser();

    if (errorCode == ERROR) {
        printf("[error]");
    }

    return 0;
}

/**
 *  Парсер логических выражений:
 *      Читает данные со стандартного ввода
 *      Позволяет задать булевы переменные перед выражением
 * @return int errorCode
 */
int run_parser() {
    dict_t **variablesDictionary = dict_alloc();
    char *line = NULL;
    size_t lineLength = 0;
    ssize_t charsCount = 0;

    do {
        charsCount = getline(&line, &lineLength, stdin);
        if (line == NULL || charsCount == -1){
            free_if_not_null(line);
            return ERROR;
        }
        remove_unnecessary_spaces(line, false);
        if (is_assignment_valid(line)) {
            add_var_to_dict(variablesDictionary, line);
        } else if (is_expression_valid(line)) {
            bool isError = false;
            bool result = parse_logical_expression(&isError, line, variablesDictionary);
            if (isError) {
                free_run_parser(line, variablesDictionary);
                return ERROR;
            }

            if (result) {
                printf("True");
            } else {
                printf("False");
            }

        } else {
            free_run_parser(line, variablesDictionary);
            return ERROR;
        }
    } while (charsCount > 0 && line[charsCount-2] == ';');

    free_run_parser(line, variablesDictionary);

    return 0;
};

bool parse_logical_expression(bool *isError, char *line, dict_t **dict) {
    node_t *rootNode = NULL;
    node_t *newNode = NULL;

    size_t allocSize = 2;
    node_t** brackets = (node_t**)malloc(allocSize * sizeof(node_t**));
    size_t bracketsCount = 0;
    int i = 0;
    int tokenStart = i;
    int tokenEnd = i;

    while (line[i] != '\0' && line[i] != '\n' && line[i] != ';') {
        if (bracketsCount == allocSize) {
            if (!increase_stack_memory(&brackets, &allocSize)) {
                *isError = true;
                return ERROR;
            }
        }

        if (line[i] == '(') {
            newNode = calloc(1, sizeof(node_t));

            newNode->operator = Brackets;
            insert_token_node(&rootNode, newNode);

            brackets[bracketsCount] = newNode;
            bracketsCount++;
        } else if (line[i] == ')') {
            bracketsCount--;
            brackets[bracketsCount]->operator = ClosedBrackets;
        } else if (line[i] != ' ') {
            tokenStart = i;
            tokenEnd = i;

            while (!is_word_braking_char(line, tokenEnd + 1)) {
                tokenEnd++;
            }

            newNode = calloc(1, sizeof(node_t));

            switch (get_operator_type(line, tokenStart, -1)) {
                case Not:
                    newNode->operator = Not;
                    break;
                case And:
                    newNode->operator = And;
                    break;
                case Or:
                    newNode->operator = Or;
                    break;
                case Xor:
                    newNode->operator = Xor;
                    break;
                case Identifier:
                    newNode->operator = Identifier;
                    int count = (tokenEnd - tokenStart) + 1;
                    char *varName = NULL;
                            varName = calloc(1, (size_t) count + 1);
                    strncpy(varName, line + tokenStart, (size_t) count);
                    if (is_true_str(line, tokenStart)) {
                        newNode->value = true;
                    } else if(is_false_str(line, tokenStart)) {
                        newNode->value = false;
                    } else {
                        int item = get_dict_item(*dict, varName);
                        if (item == ERROR){
                            *isError = true;
                            free(varName);
                            free(newNode);
                            free(brackets);
                            free_token_node_tree(rootNode);
                            return ERROR;
                        } else if (item == 1){
                            newNode->value = true;
                        } else {
                            newNode->value = false;
                        }
                    }
                    free(varName);
                    break;
                default:
                    newNode->operator = Identifier;
                    if (is_true_str(line, tokenStart)) {
                        newNode->value = true;
                    } else if(is_false_str(line, tokenStart)) {
                        newNode->value = false;
                    }
                    break;

            }
            insert_token_node(&rootNode, newNode);
            i = tokenEnd;
        }

        i++;
    }

    bool result = resolve_token_node(rootNode);

    free(brackets);
    free_token_node_tree(rootNode);

    return result;
}

bool increase_stack_memory(node_t ***stack, size_t *allocSize) {
    *allocSize *= 2;

    node_t** _stack = (node_t**)realloc(*stack, *allocSize * sizeof(node_t**));

    if (_stack != NULL) {
        *stack = _stack;
        return true;

    } else {
        free(*stack);
        return false;
    }
}


void add_var_to_dict(dict_t **dict, char *assignLine){
    if (strlen(assignLine) <= 1){
        return;
    }
    int assignOperatorIndex = find_char_in_arr(assignLine, '=');

    char *newVar = NULL;
    newVar = calloc(1, (size_t) assignOperatorIndex + 1);
    strncpy(newVar, assignLine, (size_t) assignOperatorIndex);
    bool newVarValue = false;
    if (is_true_str(assignLine, assignOperatorIndex + 1)) {
        newVarValue = true;
    } else if (is_false_str(assignLine, assignOperatorIndex + 1)) {
        newVarValue = false;
    }
    add_dict_item(dict, newVar, newVarValue);

    free(newVar);
}

void insert_token_node(node_t **root, node_t *newNode) {
    node_t *rootNode = *root;
    node_t *node = NULL;
    node = rootNode;

    if (rootNode != NULL && rootNode->operator == Brackets) {
        insert_token_node(&rootNode->primary_child, newNode);
        return;
    }

    if (!node) {
        node = newNode;
    } else if (newNode->operator != Identifier && (node->operator == Identifier || get_priority(node->operator) >= get_priority(newNode->operator))) {
        newNode->primary_child = node;
        node = newNode;
    } else { //if newNode->operator == Identifier || (node->operator != Identifier && get_priority(node->operator) < get_priority(newNode->operator)
        if (node->primary_child == NULL) {
            node->primary_child = newNode;
        }else if (node->secondary_child == NULL){
            node->secondary_child = newNode;
        } else {
            insert_token_node(&node->secondary_child, newNode);
        }
    }

    if (rootNode != NULL && rootNode->operator == Brackets) {
        rootNode->primary_child = node;
    } else {
        rootNode = node;
    }

    *root = rootNode;
}

bool resolve_token_node(node_t *node) {
    if (node == NULL){
        return false;
    }

    bool r, l = false;

    switch (node->operator) {
        case Identifier:
            return node->value;
        case Not:
            return !resolve_token_node(node->primary_child);
        case And:
            r = resolve_token_node(node->secondary_child);
            l = resolve_token_node(node->primary_child);
            return r && l;
        case Or:
            r = resolve_token_node(node->secondary_child);
            l = resolve_token_node(node->primary_child);
            return r || l;
        case Xor:
            r = resolve_token_node(node->secondary_child);
            l = resolve_token_node(node->primary_child);
            return r ^ l;
        case Brackets:
        case ClosedBrackets:
            return resolve_token_node(node->primary_child);
    }
    return false;
}

void remove_unnecessary_spaces(char *line, bool removeAll) {
    char *i = line;
    char *j = line;

    while (*j != 0) {
        *i = *j;
        j++;
        if (removeAll){
            if (*i != ' ') i++;
        } else {
            if (*i != ' ' || (*i == ' ' && *(i - 1) != ' ')) i++; // leave only one space between chars
        }
    }

    *i = 0;
}

bool is_expression_valid(char *line) {

    size_t len = strlen(line);
    if(len <= 1){
        return true;
    }

    int bracketsCount = 0;
    int i = 0;
    int tokenStart = 0;
    int tokenEnd = 0;

    int prevTokenStartIndex = 0;
    int prevTokenEndIndex = 0;

    while (line[i] != '\0' && line[i] != '\n' && line[i] != ';') {
        if (line[i] == '(') {
            bracketsCount++;
            if (i != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
                int operType = get_operator_type(line, prevTokenStartIndex, -1);
                if(operType == -1 || operType == Identifier){
                    return false;
                }
            }
        } else if (line[i] == ')') {
            bracketsCount--;
            if (i != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
                int operType = get_operator_type(line, prevTokenStartIndex, -1);
                if( operType != Identifier){
                    return false;
                }
            }
        } else if (line[i] != ' ') {
            tokenStart = i;
            tokenEnd = i;

            while (!is_word_braking_char(line, tokenEnd + 1)) {
                tokenEnd++;
            }

            switch (get_operator_type(line, tokenStart, -1)) {
                case Not:
                    // проверить на наличие переменной или константы после not
                    if (i != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
                        int operType = get_operator_type(line, prevTokenStartIndex, -1);
                        if(operType == Identifier
                        || prevTokenStartIndex != prevTokenEndIndex){ // если не ( и не )
                            return false;
                        }
                    }
                    break;
                case And:
                    // валидные примеры
                    // isdog and iscat
                    // (...) and (...)
                    // True and False
                    // isdog and True, True and iscat
                    // isdog and not iscat, isdog and not True
                    // Слева не может быть оператора
                    // справа может быть только опертор not
                case Or:
                case Xor:
                    if (i != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
                        int operType = get_operator_type(line, prevTokenStartIndex, -1);
                        if(operType != Identifier){
                            return false;
                        }
                    }
                    break;
                case Identifier:
                    // and isdog and
                    // not isdog, not False, not isdog and
                    // isdog isdog isdog
                    //  с двух сторон не может быть переменной или константы
                    if (i != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
                        int operType = get_operator_type(line, prevTokenStartIndex, -1);
                        if(operType == Identifier
                           || (operType == -1 && prevTokenStartIndex != prevTokenEndIndex)){ // если не ( и не )
                            return false;
                        }
                    }
                    break;
                default:
                    // если слово не является ни оператором или переменной или константой
                    return false;
            }
            i = tokenEnd;
            prevTokenStartIndex = tokenStart;
            prevTokenEndIndex = tokenEnd;
        }

        i++;
    }

    if (bracketsCount != 0) {
        return false;
    }

    return true;
};

bool is_word_braking_char(const char *line, int index){
    return line[index] == ' '
           || line[index] == '('
           || line[index] == ')'
           || line[index] == '\0'
           || line[index] == '\n'
           || line[index] == ';'
           || line[index] == '=';
}

/**
 * Возвращает индекс элемента только если этот
 * элемент встречается ровно один раз в строке,
 * иначе возвращает -1
 * @param line
 * @param character
 * @return
 */
int find_char_in_arr(char *line, char character) {
    int count = 0;
    int index = -1;
    for (int i = 0; line[i] != '\0'
                    && line[i] != '\n'
                    && line[i] != ';'; i++) {
        if (line[i] == character) {
            index = i;
            count++;
        };
    }
    if (count != 1) {
        return -1;
    }
    return index;
}

bool is_low_letter(char character) {
    return (character >= 'a') && (character <= 'z');
}

bool compareStr(const char *line, int firstIndex, char *str){
    size_t len = strlen(str);
    for (size_t i = 0; i < len; i++ ){
        if (line[firstIndex + i] != str[i]) {
            return false;
        }
    }
    return true;
}

bool is_true_str(char *line, int firstIndex) {
    return compareStr(line, firstIndex, "True");
}

bool is_false_str(char *line, int firstIndex) {
    return compareStr(line, firstIndex, "False");
}

bool is_var_name(char *line, int firstIndex, int lastIndex) {
    for (int i = firstIndex; i <= lastIndex; i++) {
        if (!is_low_letter(line[i])) {
            return false;
        };
    }
    return true;
};

int get_operator_type(char *line, int firstIndex, int lastIndex){
    if (compareStr(line, firstIndex, "not")){
        return Not;
    } else if (compareStr(line, firstIndex, "and")){
        return And;
    } else if (compareStr(line, firstIndex, "or")){
        return Or;
    } else if (compareStr(line, firstIndex, "xor")){
        return Xor;
    } else if (is_var_name(line, firstIndex, lastIndex) || is_true_str(line, firstIndex) ||
               is_false_str(line, firstIndex)){
        return Identifier;
    }

    return -1;
}

bool is_assignment_valid(char *line) {
    size_t len = strlen(line);
    if(len <= 1){
        return true;
    }

    int assignOperatorIndex = find_char_in_arr(line, '=');

    if (assignOperatorIndex == -1) {
        return false;
    }

    if (!is_var_name(line, 0, assignOperatorIndex - 1)) {
        return false;
    }

    if (!(is_true_str(line, assignOperatorIndex + 1)
          || is_false_str(line, assignOperatorIndex + 1))) {

        return false;
    }

    return true;
}

/**
 * Get operator's priority
 * @param operator
 * @return Higher a returned number -> higher operator's priority
 */
int get_priority(int operator) {
    switch (operator) {
        case Or:
            return 0;
        case And:
            return 1;
        case Xor:
            return 2;
        case Not:
            return 3;
        case Brackets:
        case ClosedBrackets:
            return 4;
        default:
            return -1;
    }
};

void free_if_not_null(char *line) {
    if (line != NULL) free(line);
};

void free_run_parser(char *line, dict_t **variablesDictionary){
    free_if_not_null(line);
    dict_dealloc(variablesDictionary);
}

void free_token_node_tree(node_t *rootNode){
    if (rootNode != NULL){
        free_token_node_tree(rootNode->primary_child);
        free_token_node_tree(rootNode->secondary_child);
        free(rootNode);
    }
}

// =============================== MAP
dict_t **dict_alloc(void) {
    return calloc(1, sizeof(dict_t));
}

void dict_dealloc(dict_t **dict) {
    dict_t *ptr, *next;
    for(ptr = *dict; ptr != NULL; ptr = next){
        next = ptr->next;
        free(ptr->key);
        free(ptr);
    }
    free(dict);
}

int get_dict_item(dict_t *dict, char *key) {
    dict_t *ptr;
    for (ptr = dict; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            if(ptr->value){
                return 1;
            }
            return 0;
        }
    }

    return ERROR;
}

void del_dict_item(dict_t **dict, char *key) {
    dict_t *ptr, *prev;
    for (ptr = *dict, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            if (ptr->next != NULL) {
                if (prev == NULL) {
                    *dict = ptr->next;
                } else {
                    prev->next = ptr->next;
                }
            } else if (prev != NULL) {
                prev->next = NULL;
            } else {
                *dict = NULL;
            }

            free(ptr->key);
            free(ptr);

            return;
        }
    }
}

void add_dict_item(dict_t **dict, char *key, bool value) {
    del_dict_item(dict, key); /* Если у нас уже есть элемент с такми ключом, то удалим его. */
    dict_t *d = malloc(sizeof(struct dict_t_struct));
    d->key = malloc(strlen(key) + 1);
    strcpy(d->key, key);
    d->value = value;
    d->next = *dict;
    *dict = d;
}
