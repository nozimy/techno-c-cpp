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
 *          то "(", ")", "iscat", "and", "not", "isdog" - это токены (token)
 *      строится дерево токенов с учетом приоритетов операций
 *      начиная с корневого элемента разрешается это дерево, результат - булево значение
 *      если есть переменные, то они хранятся в словаре переменных variablesDictionary
 *      также производится валидация каждой строки перед выполнением операций сохранения переменных
 *          или вычисления логического выражения
 */


/* Для хранения переменных iscat, isdog и т.д. */
typedef struct dictionary {
    char *key;
    bool value;
    struct dictionary *next;
} dictionary;

dictionary **dict_alloc(void);
void dict_dealloc(dictionary **dict);
int get_dict_item(const dictionary *dict, const char *key);
void del_dict_item(dictionary **dict, const char *key);
int add_dict_item(dictionary **dict, const char *key, bool value);
int add_var_to_dict(const char *assignLine, dictionary **dict);

enum operators {
    OR = 0, AND = 1, XOR = 2,  NOT = 3, OPEN_BRACKET = 4, CLOSED_BRACKET = 5, IDENTIFIER, CONST_KEYWORD
};

enum reports {
    ERROR = -1, NO_ERROR = 0
};
static const char *TRUE_STRING = "True";
static const char *FALSE_STRING = "False";
static const char *NOT_STRING = "not";
static const char *OR_STRING = "or";
static const char *XOR_STRING = "xor";
static const char *AND_STRING = "and";
static const char OPEN_BRACKET_SYMBOL = '(';
static const char CLOSED_BRACKET_SYMBOL = ')';
static const char EQUALITY_SYMBOL = '=';
static const char LETTER_A = 'a';
static const char LETTER_Z = 'z';
static const char END_OF_LINE_SYMBOL = '\n';
static const char END_OF_STRING_SYMBOL = '\0';
static const char SPACE_SYMBOL = ' ';
static const char SEMICOLON_SYMBOL = ';';


typedef struct token_node {
    enum operators operator;
    bool value;
    struct token_node *primary_child;
    struct token_node *secondary_child;
} token_node;

int run_parser();
void remove_unnecessary_spaces(char *line, bool removeAll);
bool is_assignment_valid(const char * line);
bool is_expression_valid(const char * line);
bool parse_logical_expression(const char *line, const dictionary *dict, bool *isError);

void insert_token_node(token_node **root, token_node *newNode);
bool resolve_token_node(const token_node *node);

int get_operator_type(const char *line, int firstIndex, int lastIndex);

bool is_low_letter(char character);
bool is_var_name(const char *line, int firstIndex, int lastIndex);
bool is_true_str(const char *line, int firstIndex);
bool is_false_str(const char *line, int firstIndex);
int find_char_in_arr(const char *line, char character);
bool is_word_braking_char(const char *line, int index);

bool increase_stack_memory(token_node ***stack, size_t *allocSize);

void free_if_not_null(char *line);
void free_run_parser(char *line, dictionary **variablesDictionary);
void free_token_node_tree(token_node *rootNode);

int fill_node(const char *line, const dictionary *dict, int tokenStart, int tokenEnd, token_node *newNode);
bool is_bracket_valid(const char *line, int index, int prevTokenStartIndex, int prevTokenEndIndex, char bracket);
bool is_tokennot_valid(const char *line, int index, int prevTokenStartIndex, int prevTokenEndIndex);
bool is_and_xor_or_valid(const char *line, int index, int prevTokenStartIndex, int prevTokenEndIndex);

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
    dictionary **variablesDictionary = dict_alloc();
    if (variablesDictionary == NULL) {
        return ERROR;
    }

    char *line = NULL;
    size_t lineLength = 0;
    ssize_t charsCount = 0;
    int result = NO_ERROR;

    do {
        charsCount = getline(&line, &lineLength, stdin);
        if (line == NULL || charsCount == -1){
            free_if_not_null(line);
            result = ERROR;
        }
        if (result == NO_ERROR) {
            remove_unnecessary_spaces(line, false);
            if (is_assignment_valid(line)) {
                result = add_var_to_dict(line, variablesDictionary);
            } else if (is_expression_valid(line)) {
                bool isError = false;
                bool product = parse_logical_expression(line, *variablesDictionary, &isError);
                if (isError) {
                    result = ERROR;
                }
                if (result == NO_ERROR) {
                    if (product) {
                        printf("%s", TRUE_STRING);
                    } else {
                        printf("%s", FALSE_STRING);
                    }
                }
            } else {
                result = ERROR;
            }
        }
    } while (result == NO_ERROR && charsCount > 0 && line[charsCount-2] == SEMICOLON_SYMBOL);

    free_run_parser(line, variablesDictionary);

    return result;
};

int fill_node(const char *line, const dictionary *dict, int tokenStart, int tokenEnd, token_node *newNode){
    int result = NO_ERROR;

    newNode->operator = get_operator_type(line, tokenStart, -1);

    if (newNode->operator == IDENTIFIER) {
        int count = (tokenEnd - tokenStart) + 1;
        char *varName = NULL;
        varName = calloc(1, (size_t) count + 1);
        if (varName == NULL) {
            result = ERROR;
        } else {
            strncpy(varName, line + tokenStart, (size_t) count);
            if (is_true_str(line, tokenStart)) {
                newNode->value = true;
            } else if(is_false_str(line, tokenStart)) {
                newNode->value = false;
            } else {
                int item = get_dict_item(dict, varName);
                if (item == ERROR){
                    result = ERROR;
                } else if (item == 1){
                    newNode->value = true;
                } else {
                    newNode->value = false;
                }
            }
            free(varName);
        }
    } else if (newNode->operator == CONST_KEYWORD) {
        newNode->operator = IDENTIFIER;
        if (is_true_str(line, tokenStart)) {
            newNode->value = true;
        } else if(is_false_str(line, tokenStart)) {
            newNode->value = false;
        }
    }

    return result;
}

bool parse_logical_expression(const char *line, const dictionary *dict, bool *isError) {
    token_node *rootNode = NULL;
    token_node *newNode = NULL;

    size_t allocSize = 2;
    token_node** brackets = (token_node**)malloc(allocSize * sizeof(token_node**));
    if (brackets == NULL) {
        *isError = true;
        return false;
    }

    size_t bracketsCount = 0;
    int i = 0;
    int tokenStart = i;
    int tokenEnd = i;

    while (!*isError && line[i] != END_OF_STRING_SYMBOL && line[i] != END_OF_LINE_SYMBOL && line[i] != SEMICOLON_SYMBOL) {
        if (bracketsCount == allocSize) {
            if (!increase_stack_memory(&brackets, &allocSize)) {
                *isError = true;
            }
        }

        if (!*isError) {
            if (line[i] == OPEN_BRACKET_SYMBOL) {
                newNode = calloc(1, sizeof(token_node));
                if (newNode == NULL) {
                    *isError = true;
                } else {
                    newNode->operator = OPEN_BRACKET;
                    insert_token_node(&rootNode, newNode);

                    brackets[bracketsCount] = newNode;
                    bracketsCount++;
                }
            } else if (line[i] == CLOSED_BRACKET_SYMBOL) {
                bracketsCount--;
                brackets[bracketsCount]->operator = CLOSED_BRACKET;
            } else if (line[i] != SPACE_SYMBOL) {
                tokenStart = i;
                tokenEnd = i;

                while (!is_word_braking_char(line, tokenEnd + 1)) {
                    tokenEnd++;
                }

                newNode = calloc(1, sizeof(token_node));
                if (newNode == NULL) {
                    *isError = true;
                } else {
                    int err = fill_node(line, dict, tokenStart, tokenEnd, newNode);
                    if (err == ERROR) {
                        *isError = true;
                    }
                }

                if (!*isError) {
                    insert_token_node(&rootNode, newNode);
                    i = tokenEnd;
                }
            }
            i++;
        }
    }

    if (*isError) {
        free(newNode);
        free(brackets);
        free_token_node_tree(rootNode);
        return ERROR;
    }

    bool result = resolve_token_node(rootNode);
    free(brackets);
    free_token_node_tree(rootNode);

    return result;
}

bool increase_stack_memory(token_node ***stack, size_t *allocSize) {
    if (allocSize == NULL) {
        if (stack != NULL) {
            free(*stack);
        }
        return false;
    }

    *allocSize *= 2;

    token_node** _stack = (token_node**)realloc(*stack, *allocSize * sizeof(token_node**));

    if (_stack == NULL) {
        free(*stack);
        return false;
    }
    *stack = _stack;
    return true;
}


int add_var_to_dict(const char *assignLine, dictionary **dict){
    if (strlen(assignLine) <= 1){
        return ERROR;
    }
    int assignOperatorIndex = find_char_in_arr(assignLine, EQUALITY_SYMBOL);

    char *newVar = NULL;
    newVar = calloc(1, (size_t) assignOperatorIndex + 1);
    if (newVar == NULL) {
        return ERROR;
    }
    strncpy(newVar, assignLine, (size_t) assignOperatorIndex);
    bool newVarValue = false;
    if (is_true_str(assignLine, assignOperatorIndex + 1)) {
        newVarValue = true;
    } else if (is_false_str(assignLine, assignOperatorIndex + 1)) {
        newVarValue = false;
    }
    int err = add_dict_item(dict, newVar, newVarValue);

    free(newVar);

    if (err == ERROR) {
        return ERROR;
    }

    return NO_ERROR;
}

void insert_token_node(token_node **root, token_node *newNode) {
    token_node *rootNode = *root;
    token_node *node = NULL;
    node = rootNode;

    if (rootNode != NULL && rootNode->operator == OPEN_BRACKET) {
        insert_token_node(&rootNode->primary_child, newNode);
        return;
    }

    if (!node) {
        node = newNode;
    } else if (newNode->operator != IDENTIFIER && (node->operator == IDENTIFIER
    || node->operator >= newNode->operator)) {
        newNode->primary_child = node;
        node = newNode;
    } else {
        if (node->primary_child == NULL) {
            node->primary_child = newNode;
        }else if (node->secondary_child == NULL){
            node->secondary_child = newNode;
        } else {
            insert_token_node(&node->secondary_child, newNode);
        }
    }

    if (rootNode != NULL && rootNode->operator == OPEN_BRACKET) {
        rootNode->primary_child = node;
    } else {
        rootNode = node;
    }

    *root = rootNode;
}

bool resolve_token_node(const token_node *node) {
    if (node == NULL){
        return false;
    }

    bool r, l = false;

    switch (node->operator) {
        case IDENTIFIER:
            return node->value;
        case NOT:
            return !resolve_token_node(node->primary_child);
        case AND:
            r = resolve_token_node(node->secondary_child);
            l = resolve_token_node(node->primary_child);
            return r && l;
        case OR:
            r = resolve_token_node(node->secondary_child);
            l = resolve_token_node(node->primary_child);
            return r || l;
        case XOR:
            r = resolve_token_node(node->secondary_child);
            l = resolve_token_node(node->primary_child);
            return r ^ l;
        case OPEN_BRACKET:
        case CLOSED_BRACKET:
            return resolve_token_node(node->primary_child);
        case CONST_KEYWORD:break;
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
            if (*i != SPACE_SYMBOL) i++;
        } else {
            if (*i != SPACE_SYMBOL || (*i == SPACE_SYMBOL && *(i - 1) != SPACE_SYMBOL)) i++; // leave only one space between chars
        }
    }

    *i = 0;
}

bool is_bracket_valid(const char *line, int index, int prevTokenStartIndex, int prevTokenEndIndex, char bracket){
    if (index != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
        int operType = get_operator_type(line, prevTokenStartIndex, -1);
        if (bracket == OPEN_BRACKET_SYMBOL) {
            if(operType == CONST_KEYWORD || operType == IDENTIFIER){
                return false;
            }
        } else if (bracket == CLOSED_BRACKET_SYMBOL) {
            if( operType != IDENTIFIER){
                return false;
            }
        }
    }
    return true;
}

// проверить на наличие переменной или константы после not
bool is_tokennot_valid(const char *line, int index, int prevTokenStartIndex, int prevTokenEndIndex){
    if (index != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
        int operType = get_operator_type(line, prevTokenStartIndex, -1);
        if(operType == IDENTIFIER
           || prevTokenStartIndex != prevTokenEndIndex){ // если не ( и не )
            return false;
        }
    }
    return true;
}

/**
 * валидные примеры
 * isdog and iscat
 * (...) and (...)
 * True and False
 * isdog and True, True and iscat
 * isdog and not iscat, isdog and not True
 * Слева не может быть оператора
 * справа может быть только опертор not
 */
bool is_and_xor_or_valid(const char *line, int index, int prevTokenStartIndex, int prevTokenEndIndex){
    if (index != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
        int operType = get_operator_type(line, prevTokenStartIndex, -1);
        if(operType != IDENTIFIER){
            return false;
        }
    }
    return true;
}

/**
 * and isdog and
 * not isdog, not False, not isdog and
 * isdog isdog isdog
 * с двух сторон не может быть переменной или константы
 */
bool is_identifier_valid(const char *line, int index, int prevTokenStartIndex, int prevTokenEndIndex){
    if (index != 0 && prevTokenEndIndex !=0 && prevTokenStartIndex != 0) {
        int operType = get_operator_type(line, prevTokenStartIndex, -1);
        if(operType == IDENTIFIER
           || (operType == CONST_KEYWORD && prevTokenStartIndex != prevTokenEndIndex)){ // если не ( и не )
            return false;
        }
    }
    return true;
}

bool is_expression_valid(const char *line) {
    bool result = true;

    size_t len = strlen(line);
    if(len <= 1){
        return result;
    }

    int bracketsCount = 0;
    int i = 0;
    int tokenStart = 0;
    int tokenEnd = 0;

    int prevTokenStartIndex = 0;
    int prevTokenEndIndex = 0;

    while (result && line[i] != END_OF_STRING_SYMBOL && line[i] != END_OF_LINE_SYMBOL && line[i] != SEMICOLON_SYMBOL) {
        if (line[i] == OPEN_BRACKET_SYMBOL) {
            bracketsCount++;
            result = is_bracket_valid(line, i, prevTokenStartIndex, prevTokenEndIndex, line[i]);
        } else if (line[i] == CLOSED_BRACKET_SYMBOL) {
            bracketsCount--;
            result = is_bracket_valid(line, i, prevTokenStartIndex, prevTokenEndIndex, line[i]);
        } else if (line[i] != SPACE_SYMBOL) {
            tokenStart = i;
            tokenEnd = i;

            while (!is_word_braking_char(line, tokenEnd + 1)) {
                tokenEnd++;
            }

            switch (get_operator_type(line, tokenStart, -1)) {
                case NOT:
                    result = is_tokennot_valid(line, i, prevTokenStartIndex, prevTokenEndIndex);
                    break;
                case AND:
                case OR:
                case XOR:
                    result = is_and_xor_or_valid(line, i, prevTokenStartIndex, prevTokenEndIndex);
                    break;
                case IDENTIFIER:
                    result = is_identifier_valid(line, i, prevTokenStartIndex, prevTokenEndIndex);
                    break;
                default:
                    // если слово не является ни оператором или переменной или константой
                    result = false;
            }
            i = tokenEnd;
            prevTokenStartIndex = tokenStart;
            prevTokenEndIndex = tokenEnd;
        }

        i++;
    }

    if (bracketsCount != 0) {
        result = false;
    }

    return result;
};

bool is_word_braking_char(const char *line, int index){
    return line[index] == SPACE_SYMBOL
           || line[index] == OPEN_BRACKET_SYMBOL
           || line[index] == CLOSED_BRACKET_SYMBOL
           || line[index] == END_OF_STRING_SYMBOL
           || line[index] == END_OF_LINE_SYMBOL
           || line[index] == SEMICOLON_SYMBOL
           || line[index] == EQUALITY_SYMBOL;
}

/**
 * Возвращает индекс элемента только если этот
 * элемент встречается ровно один раз в строке,
 * иначе возвращает -1
 * @param line
 * @param character
 * @return
 */
int find_char_in_arr(const char *line, char character) {
    int count = 0;
    int index = -1;
    for (int i = 0; line[i] != END_OF_STRING_SYMBOL
                    && line[i] != END_OF_LINE_SYMBOL
                    && line[i] != SEMICOLON_SYMBOL; i++) {
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
    return (character >= LETTER_A) && (character <= LETTER_Z);
}

bool compareStr(const char *line, int firstIndex, const char *str){
    size_t len = strlen(str);
    bool result = true;
    for (size_t i = 0; i < len && result; i++ ){
        if (line[firstIndex + i] != str[i]) {
            result = false;
        }
    }
    return result;
}

bool is_true_str(const char *line, int firstIndex) {
    return compareStr(line, firstIndex, TRUE_STRING);
}

bool is_false_str(const char *line, int firstIndex) {
    return compareStr(line, firstIndex, FALSE_STRING);
}

bool is_var_name(const char *line, int firstIndex, int lastIndex) {
    for (int i = firstIndex; i <= lastIndex; i++) {
        if (!is_low_letter(line[i])) {
            return false;
        };
    }
    return true;
};

int get_operator_type(const char *line, int firstIndex, int lastIndex){
    if (compareStr(line, firstIndex, NOT_STRING)){
        return NOT;
    } else if (compareStr(line, firstIndex, AND_STRING)){
        return AND;
    } else if (compareStr(line, firstIndex, OR_STRING)){
        return OR;
    } else if (compareStr(line, firstIndex, XOR_STRING)){
        return XOR;
    } else if (is_var_name(line, firstIndex, lastIndex) || is_true_str(line, firstIndex) ||
               is_false_str(line, firstIndex)){
        return IDENTIFIER;
    }

    return CONST_KEYWORD;
}

bool is_assignment_valid(const char *line) {
    size_t len = strlen(line);
    if(len <= 1){
        return true;
    }
    int assignOperatorIndex = find_char_in_arr(line, EQUALITY_SYMBOL);
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

void free_if_not_null(char *line) {
    if (line != NULL) free(line);
};

void free_run_parser(char *line, dictionary **variablesDictionary){
    free_if_not_null(line);
    dict_dealloc(variablesDictionary);
}

void free_token_node_tree(token_node *rootNode){
    if (rootNode != NULL){
        free_token_node_tree(rootNode->primary_child);
        free_token_node_tree(rootNode->secondary_child);
        free(rootNode);
    }
}

// =============================== MAP
dictionary **dict_alloc(void) {
    return calloc(1, sizeof(dictionary));
}

void dict_dealloc(dictionary **dict) {
    dictionary *ptr, *next;
    for(ptr = *dict; ptr != NULL; ptr = next){
        next = ptr->next;
        free(ptr->key);
        free(ptr);
    }
    free(dict);
}

int get_dict_item(const dictionary *dict, const char *key) {
    const dictionary *ptr;
    int result = ERROR;
    for (ptr = dict; ptr != NULL && result == ERROR; ptr = ptr->next) {
        if (strcmp(ptr->key, key) == 0) {
            result = 0;
            if(ptr->value){
                result = 1;
            }
        }
    }
    return result;
}

void del_dict_item(dictionary **dict, const char *key) {
    dictionary *ptr, *prev;
    bool key_found = false;
    for (ptr = *dict, prev = NULL; ptr != NULL && !key_found; prev = ptr, ptr = ptr->next) {
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

            key_found = true;
        }
    }
}

int add_dict_item(dictionary **dict, const char *key, bool value) {
    del_dict_item(dict, key); /* Если у нас уже есть элемент с такми ключом, то удалим его. */
    dictionary *d = malloc(sizeof(struct dictionary));
    if (d == NULL) {
        return ERROR;
    }
    d->key = malloc(strlen(key) + 1);
    if (d->key == NULL) {
        free(d);
        return ERROR;
    }
    strcpy(d->key, key);
    d->value = value;
    d->next = *dict;
    *dict = d;
    return NO_ERROR;
}
