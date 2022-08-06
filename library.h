#ifndef CHELPS_LIBRARY_H
#define CHELPS_LIBRARY_H

#define MINDIM 2
#define MAXCOMMAND_DIM 15
#define MAXWORD_LIST 20

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef union {
    int _n;
    char _c;
} data;
typedef enum {
    char_t = 0,
    int_t = 1,
    list_t = 2,
    n_char_t = 3,
    n_int_t = 4,
    n_list_t = 5,
    unlimited = 6
} type;

typedef struct conditions_list {
    struct conditions_list *prev;
    data data;
    type type;
    struct conditions_list *next;
} cl;

typedef struct {
    _Bool success;
    int index;
    int *indexes;
} match;

int numchecker(char num);
int xtdnumchecker(const char nums[] , _Bool skipchars);
_Bool word_comparison(const char string[], const char string_[]);
int detect(int line, const char func[], _Bool timecheck);
void cString(char **getins, int DIM, _Bool fixedDim, const char conditions[], const char message[], int caller);
_Bool sString (char **getIns, const char words[], const char commands[]);
_Bool ccLess_Comparison(const char string[], const char string_[]);
void freeIt(char **var);
match *contains(char *_chars, char *_str, _Bool caseSensitive);
int **checkMatches(match **matches);
char *numToString(int num);
char *charToString(const char _char);
void bubbleSortInt(int **array, int dimension);

#define xtdynmem(type,size) (type*)calloc(size,sizeof(type))
#define caller_ detect(__LINE__,__func__,false)
#define caller_t detect(__LINE__,__func__,true)

#endif //CHELPS_LIBRARY_H
