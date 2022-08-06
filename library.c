#include "library.h"

/* ###################################################################################
 * CHAR CONVERTERS TO NUMBERS FUNCTIONS
 * ###################################################################################
 */

int numchecker(const char num) {
    if (num >= '0' && num <= '9') {
        return (int)num-48;
    }
    else return 0;
}

int xtdnumchecker(const char nums[] , _Bool skipchars) {
    int cnum = 0;
    if (skipchars == false) {
        for (int i = 0; nums[i] >= '0' && nums[i] <= '9'; i++) {
            if (cnum == 0) cnum = numchecker(nums[i]);
            else cnum = cnum * 10 + numchecker(nums[i]);
        }
    }
    else {
        for (int j = 0; j < (int)strlen(nums); j++) {
            for (int i = j; nums[i] >= '0' && nums[i] <= '9'; i++) {
                if (cnum == 0) cnum = numchecker(nums[i]);
                else cnum = cnum * 10 + numchecker(nums[i]);
                j = i;
            }
        }
    }
    return cnum;
}

int detect(const int line, const char func[], _Bool timecheck) {
    static int info_line = -1;
    static char info_func[] = "Not_A_Function_Just_Initialization";
    static int call_id = 0;
    if (info_line != line || strcmp(info_func, func) != 0) {
        info_line = line;
        strcpy(info_func, func);
        call_id = rand();
    }
    if (timecheck == true) {
        call_id = rand();
    }
    return call_id;
}

static cl *setElement(cl *element, data data, type type) {
    cl *tmp = (cl*) calloc(1, sizeof(cl));
    (*tmp).prev = element;
    (*tmp).data = data;
    (*tmp).type = type;
    (*tmp).next = NULL;
    if (element != NULL) {
        (*element).next = tmp;
    }
    return tmp;
}

static cl *getElement(cl *list, _Bool reset) {
    static cl *ptr = NULL;
    if (ptr == NULL) ptr = list;
    else ptr = (*ptr).next;
    if (reset) ptr = NULL;
    return ptr;
}

static cl *getListTop(cl *list) {
    cl *top = list;
    while ((*top).prev != NULL) {
        top = top->prev;
    }
    return top;
}

static void *freeList(cl *list) {
    while((*list).next != NULL) {
        list = (*list).next;
    }
    cl *tmp = list;
    if ((*list).prev != NULL){
        list = (*list).prev;
        (*list).next = NULL;
    }
    else {
        free(tmp);
        return NULL;
    }
    free(tmp);
    freeList(list);
}

static int getInsertion(char **getIns, int DIM, const _Bool fixedDim, const char message[]) {
    int actualDim;
    _Bool valid = false;
    while (!valid) {
        if (message != NULL) printf("%s", message);
        if (*getIns == NULL) *getIns = (char*) calloc(DIM, sizeof(char));
        scanf("%[^\n]s", *getIns);
        getchar();
        (*getIns)[DIM-1] = '\0';
        if ((*getIns)[0] != '\0'){
            if (strlen(*getIns) < DIM && !fixedDim) {
                *getIns = (char *) realloc(*getIns, (strlen(*getIns) + 1) * sizeof(char));
                actualDim = (int)strlen(*getIns);
                (*getIns)[actualDim] = '\0';
                valid = true;
            } else if (strlen(*getIns) == (DIM) - 1) {
                actualDim = DIM - 1;
                (*getIns)[actualDim] = '\0';
                valid = true;
            } else {
                freeIt(getIns);
            }
        }
    }
    return actualDim;
}

static _Bool checkIfSpecial(const char _char) {
    return (_char == '!'
            || _char == '.'
            || _char == '('
            || _char == ')'
            || _char == '|'
            || _char == '-'
            || _char == '>'
            || _char == '^'
    );
}

static _Bool checkIfNumber(const char _char) {
    return (_char >= '0' && _char <= '9');
}

/*
 * !(A|Z) negated char range
 * (c|c) char range
 * (..^!?^^zxy) char list
 * !(..^!?^zxy) negated char list
 * (0->) unlimited int
 * (^1|z) negated conversion symbol
 * (-1) incoming
 */

static void regexChars(cl **list, const char conditions[], int *pos, _Bool negated) {
    data data;
    data._c = '\0';
    type type;
    for (int i = *pos; conditions[i] != ')'; i++) {
        if (conditions[i] == '|') i++;
        if (conditions[i] == '^') i++;
        type = char_t;
        if (negated) type = n_char_t;
        data._c = conditions[i];
        *list = setElement(*list, data, type);
        *pos = i;
    }
}

static void regexIntegers(cl **list, const char conditions[], int *pos, _Bool negated) {
    data data;
    data._n = 0;
    type type;
    for (int i = *pos; conditions[i] != ')'; i++) {
        for (int j = i; checkIfNumber(conditions[j]); j++){
            if (data._n == 0) {
                data._n = numchecker(conditions[j]);
            } else {
                data._n = data._n * 10 + numchecker(conditions[j]);
            }
            type = int_t;
            if (negated) type = n_int_t;
            i = j;
        }
        if (conditions[i+1] == '-' && conditions[i+2] == '>') {
            type = unlimited;
            i += 2;
        }
        if (conditions[i] != '|') {
            *list = setElement(*list, data, type);
            data._n = 0;
            *pos = i + 1;
        }
    }
}

static void regexCompiler(cl **list, const char conditions[]) {
    _Bool negated = false;
    data data;
    type type;
    for (int i = 0; i <= strlen(conditions)-1;) {
        if (conditions[i] == ')') {
            if (i == strlen(conditions)-1) break;
            else i++;
        }
        if (conditions[i] == '!') {
            negated = true;
            i++;
        }
        i++;
        if (conditions[i] == '.') {
            for (int j = i+2, ignoreSymbol = 0; true; j++) {
                if (conditions[j] == ')' && !ignoreSymbol) {
                    i = j;
                    break;
                }
                if (conditions[j] == '^') {
                    j++;
                }
                type = list_t;
                if (negated) type = n_list_t;
                data._c = conditions[j];
                *list = setElement(*list, data, type);
            }
        }
        else if (checkIfNumber(conditions[i])) {
            regexIntegers(&(*list), conditions, &i, negated);
        }
        else {
            regexChars(&(*list), conditions, &i, negated);
        }
        if (negated) negated = false;
    }
    *list = getListTop(*list);
}

/*
 * ERROR CODES:
 * - 89: less parenthesis than expected
 * - -89: more parenthesis than expected
 * - 46: expected more '.' to define a list
 * - 124: No range extremities separator
 * - -33: Wrong negation symbol position
 * - 33: Illegal argument
 * "(A|Z)(..!"£$%&)!(..^!"£$%&)!(A|Z)"
 */

static void evaluateSyntax(const char conditions[]) {
    for (int i = 0, parenthesis = 0, verticalSlash = 0, ignoreSymbol = 0; i < strlen(conditions); i++) {
        if (parenthesis == 0 && conditions[i] != '!' && conditions[i] != '(') exit(33);
        else if (parenthesis == 0 && conditions[i] == '(') {
            parenthesis++;
            i++;
        }
        if (parenthesis == 1 && conditions[i] == '(' && conditions[i-1] != '^' && i != '0') exit(-89);
        if (parenthesis == 1 && conditions[i] == ')') {
            if (conditions[i-1] == '^'
                && (conditions[i+1] == '!' || conditions[i+1] == '(' || i+1 == strlen(conditions))
                && ignoreSymbol) exit(89);
            else  parenthesis--;
        }
        if (parenthesis) {
            if (conditions[i-1] == '(' && conditions[i] == '.' && conditions[i+1] == '.') {
                i += 2;
                for (int j = i; true; j++) {
                    if (conditions[j] == ')' && (conditions[j-1] != '^' || !ignoreSymbol)) break; // break
                    if (conditions[j] == '^') {
                        ignoreSymbol++;
                        if (!checkIfSpecial(conditions[j+1]) && !checkIfNumber(conditions[j+1])) exit(33);
                        else {
                            j++;
                            ignoreSymbol--;
                        }
                    }
                    else if (checkIfSpecial(conditions[j]) || checkIfNumber(conditions[j])) exit(33);
                    i = j;
                }
            }
            else if (conditions[i-1] == '(' && checkIfNumber(conditions[i])) {
                for (int j = i, onlyNumbers = false, unlimited = false; conditions[j] != ')' && unlimited != 1; j++) {
                    i = j;
                    if (!onlyNumbers && !unlimited) {
                        if (conditions[j] == '|' && checkIfNumber(conditions[j-1])) {
                            onlyNumbers = true;
                            j++;
                        }
                        if (conditions[j] == '-' && checkIfNumber(conditions[j-1]) && !onlyNumbers) {
                            if (conditions[j+1] == '>' && conditions[j+2] == ')') {
                                unlimited = true;
                                i = j+1;
                            }
                            else exit(33);
                        }
                        else if (!(checkIfNumber(conditions[j]))) exit(33);
                    }
                    if (onlyNumbers) {
                        if (!(checkIfNumber(conditions[j]))) {
                            exit(33);
                        }
                        else i = j;
                    }
                }
            }
            else {
                for (int j = i; true; j++) {
                    if (conditions[j] == '^') {
                        ignoreSymbol++;
                        if (checkIfSpecial(conditions[j + 1]) || checkIfNumber(conditions[j + 1])) {
                            ignoreSymbol--;
                            j++;
                        }
                        else exit(33);
                        if ((conditions[j+1] == '|' && conditions[j-2] == '(') || (conditions[j+1] == ')' && conditions[j-2] == '|')) j++;
                        else exit(33);
                    }
                    else if (checkIfSpecial(conditions[j]) || checkIfNumber(conditions[j])) exit(33);
                    else {
                        if ((conditions[j+1] == '|' && conditions[j-1] == '(') || (conditions[j+1] == ')' && conditions[j-1] == '|')) j++;
                    }
                    if (conditions[j] == ')' && (conditions[j-1] != '^' || !ignoreSymbol)) {
                        i = j-1;
                        break;
                    }
                    i = j;
                }
            }
        }
    }
}

static void negativeConditions(cl *list, cl **listPointer, const char *getIns, _Bool *validity) {
    type type = (**listPointer).type;
    _Bool invertedValidity = true;
    int num = 0;
    int position = -1;
    for (int i = 0; i < strlen(getIns) && invertedValidity; i++) {
        while (invertedValidity && (*listPointer) != NULL) {
            type = (**listPointer).type;
            switch (type) {
                case n_int_t: {
                    if (checkIfNumber(getIns[i]) && i > position) {
                        for (int j = i; checkIfNumber(getIns[j]); j++) {
                            if (num == 0) num = numchecker(getIns[j]);
                            else num = num * 10 + numchecker(getIns[j]);
                            position = j;
                        }
                        cl *tmp = getElement(list, false);
                        invertedValidity = !((**listPointer).data._n <= num && num <= (*tmp).data._n);
                    }
                }
                    break;
                case n_char_t: {
                    cl *tmp = getElement(list, false);
                    invertedValidity = !((**listPointer).data._c <= getIns[i] && getIns[i] <= (*tmp).data._c);
                }
                    break;
                case n_list_t: {
                    invertedValidity = ((**listPointer).data._c != getIns[i]);
                }
                    break;
                default: break;
            }
            *listPointer = getElement(list, false);
        }
        *listPointer = getElement(list, true);
        *listPointer = getElement(list, false);
    }
    if (!invertedValidity) *validity = invertedValidity;
    *listPointer = getElement(list, true);
}

static _Bool evaluateInsertion(const char *getIns, const int DIM,  const char conditions[], int caller) {
    static int lastCaller = 0;
    static cl *list = NULL;
    if (caller != lastCaller && conditions != NULL) {
        if (list != NULL) freeList(list);
        list = NULL;
        regexCompiler(&list, conditions);
    }
    if (conditions == NULL) {
        freeList(list);
        return true;
    }
    lastCaller = caller;
    _Bool validity = true;
    cl *listPointer = getElement(list, false);
    negativeConditions(list, &listPointer, getIns, &validity);
    if (listPointer == NULL) listPointer = getElement(list, false);
    for (int i = 0; i < DIM && validity; i++) {
        do {
            validity = false;
            if (listPointer == NULL) listPointer = getElement(list, false);
            type type = (*listPointer).type;
            switch(type) {
                case char_t: {
                    cl *tmp = getElement(list, false);
                    validity = ((*listPointer).data._c <= getIns[i] && getIns[i] <=(*tmp).data._c);
                }
                    break;
                case int_t:
                case unlimited: {
                    int num = -1;
                    for (int j = i; '0' <= getIns[j] && getIns[j] <= '9'; j++) {
                        if (num == -1) num = numchecker(getIns[j]);
                        else num = num * 10 + numchecker(getIns[j]);
                        i = j;
                    }
                    if (type == int_t) {
                        cl *tmp = getElement(list, false);
                        validity = ((*listPointer).data._n <= num && num <= (*tmp).data._n);
                    }
                    else {
                        validity = ((*listPointer).data._n <= num && num != -1);
                    }
                }
                    break;
                case list_t: {
                    while ((*listPointer).type == list_t && (*listPointer).type == type) {
                        validity = ((*listPointer).data._c == getIns[i]);
                        if (validity) break;
                        listPointer = getElement(list, false);
                        if (listPointer == NULL) break;
                    }
                }
                    break;
                default: break;
            }
            if (listPointer != NULL && type != list_t) listPointer = getElement(list, false);
        } while (listPointer != NULL && !validity);
        listPointer = getElement(list, true);
    }
    listPointer = getElement(list, true);
    return validity;
}

/**
 * @param getIns In this section you have to pass the address of a pointer with no memory allocated that points to NULL.
 * @param DIM This is the maximum dimension of the input that you are going to receive from the user.
 * @param fixedDim Here you can specify if the dimension is fixed or the input can have a lower dimension.
 * @param conditions This is the parameter where you have to specify the conditions of an acceptable input.
 * @param message This is the message that would be shown to the user when he has to give the input.
 * @param caller This is a special section dedicated to the macro caller_ or caller_t
 *
 * @return This function has no return but it modify the pointer that you passed in the first parameter slot and gives you\n
 * the input of the user.
 */

void cString(char **getIns, int DIM, _Bool fixedDim, const char conditions[], const char message[], const int caller) {
    if (conditions != NULL && conditions[0] != 0 && !word_comparison(conditions, "reset")) {
        evaluateSyntax(conditions);
    }
    else if (word_comparison(conditions, "reset")) {
        evaluateInsertion(NULL, 0, NULL, caller);
        return;
    }
    DIM++;
    static int lastCaller = 0;
    if (*getIns != NULL) freeIt(&(*getIns));
    if (caller != lastCaller) {
        *getIns = (char *) calloc(DIM, sizeof(char));
    }
    _Bool breaks = false;
    while (!breaks) {
        int actualDim = 0;
        if (!word_comparison(conditions, "reset")) {
            actualDim = getInsertion(getIns, DIM, fixedDim, message);
        }
        if (evaluateInsertion(*getIns, actualDim, conditions, caller)) breaks = true;
        else freeIt(&(*getIns));
    }
}

void freeIt(char **var) {
    if (*var != NULL) free(*var);
    *var = NULL;
}

_Bool word_comparison(const char string[], const char string_[]) {
    if (strlen(string) != strlen(string_)) return false;
    for (int i = 0; string[i] >= ' ' && string_[i] >= ' ' && string[i] <= 'z' && string_[i] <= 'z'; i++) {
        if ((string[i] != string_[i])) return false;
    }
    return true;
}

void s_Commands_Exec(const char commands[], char **getIns) {
    unsigned int ccount = 0;
    unsigned int progression = 0;
    char *command;
    for (int i = 0; i < (int)strlen(commands); i++) {
        if (commands[i] == ';') ccount++;
    }
    for (int i = 0; i < ccount; i++) {
        command = (char*) calloc(MAXCOMMAND_DIM, sizeof(char));
        for (unsigned int j = 0; j < (int)strlen(commands) && commands[i] != ';'; j++) {
            command[j] = commands[j+progression];
            if (commands[j+progression+1] == ';') {
                progression = progression+j+2;
                j = (unsigned int)strlen(commands);
            }
        }
        if (word_comparison(command, "rispaces") == 1) {
            char temporary = getIns[0][0];
            while (temporary == ' ') {
                for (int k = 1; k < (int) strlen(getIns[0]); k++) {
                    getIns[0][k - 1] = getIns[0][k];
                }
                getIns[0] = (char *) realloc(getIns[0], sizeof(char) * ((int) strlen(getIns[0]) - 1));
                temporary = getIns[0][0];
            }
        }
        else if (word_comparison(command, "rcspaces") == 1) {
            for (int k = 0; k < (int)strlen(getIns[0]); k++) {
                if (getIns[0][k] == ' ' && getIns[0][k+1] == ' ') {
                    for (int j = k + 1; j < (int) strlen(getIns[0]); j++) {
                        getIns[0][j] = getIns[0][j + 1];
                    }
                    getIns[0] = (char*) realloc(getIns[0], sizeof(char)*((int)strlen(getIns[0])-1));
                    k--;
                }
            }
        }
        else if (word_comparison(command, "raspaces") == 1) {
            for (int k = 0; k < (int)strlen(getIns[0]); k++) {
                if (getIns[0][k] == ' ') {
                    for (int j = k; j < (int) strlen(getIns[0]); j++) {
                        getIns[0][j] = getIns[0][j + 1];
                    }
                    getIns[0] = (char*) realloc(getIns[0], sizeof(char)*((int)strlen(getIns[0])-1));
                    k--;
                }
            }
        }
        else if (word_comparison(command, "capsinits") == 1) {
            if (getIns[0][0] >= 'a' && getIns[0][0] <= 'z') {
                getIns[0][0] -= 32;
            }
        }
        else if (word_comparison(command, "capsinitw") == 1) {
            for (int k = 0; k < (int)strlen(getIns[0]); k++) {
                if (getIns[0][k] >= 'a' && getIns[0][k] <= 'z' && (getIns[0][k-1] == ' ' || k == 0)) {
                    getIns[0][k] -= 32;
                }
            }
        }
        else if (word_comparison(command, "allcaps") == 1) {
            for (int k = 0; k < (int)strlen(getIns[0]); k++) {
                if (getIns[0][k] >= 'a' && getIns[0][k] <= 'z') getIns[0][k] -= 32;
            }
        }
        else if (word_comparison(command, "nocaps") == 1) {
            for (int k = 0; k < (int)strlen(getIns[0]); k++) {
                if (getIns[0][k] >= 'A' && getIns[0][k] <= 'Z') getIns[0][k] += 32;
            }
        }
        else if (word_comparison(command, "classicalnames") == 1) {
            for(int k = 0; k < (int)strlen(getIns[0]); k++) {
                if (getIns[0][k] >= 'A' && getIns[0][k] <= 'Z' && (getIns[0][k-1] != ' ' || k != 0)) getIns[k] += 32;
                else if (getIns[0][k] >= 'a' && getIns[0][k] <= 'z' && (getIns[0][k-1] == ' ' || k == 0)) getIns[k] -= 32;
            }
        }
        else exit(EXIT_FAILURE);
    }
    free(command);
}

static _Bool AnalyzeWords(const char words[], char getIns[]) {
    unsigned int wcount = 0;
    int progression = 0, *wlenght = (int*) calloc(MAXWORD_LIST, sizeof(int));
    _Bool isit;
    char *word;
    for (int i = 0; i < (int)strlen(words); i++) {
        if (words[i] == ';') {
            wcount++;
            wlenght[wcount-1] = i;
        }
        if (words[i+1] == '\0') {
            wlenght = realloc(wlenght,sizeof(int)*wcount);
        }
    }
    for (int i = 0; i < wcount; i++) {
        word = calloc(wlenght[i]+1,sizeof(char));
        for (int j = progression; j < (int)strlen(words); j++) {
            word[j - progression] = words[j];
            if (words[j+1] == ';') {
                progression = j+2;
                j = (int)strlen(words);
            }
        }
        word[strlen(word)] = '\0';
        if (word_comparison(getIns, word) == 1) {
            free(word);
            return true;
        }
        else {
            isit = false;
            free(word);
        }
    }
    return isit;
}

/**
 * @details This function only accepts dynamically allocated variables and arrays\n
 * You can use the MACRO NULL for the words and commands field to let them Empty!\n
 * You can concatenate commands and words: "command;command;" "word;word;"
 * @commands
 * rispaces;     -> Removes all initial spaces of the string;\n
 * rcspaces;     -> Removes multiple spaces between words of the array;\n
 * raspaces;     -> Removes all spaces;\n
 * capsinits;    -> Converts to uppercase any initial character of any string between 97 (a) and 122 (z) ASCII code;\n
 * capsinitw;    -> Converts to uppercase any initial character of any word between 97 (a) and 122 (z) ASCII code;\n
 * allcaps;      -> All lowercase become uppercase;\n
 * nocaps;       -> All uppercase become lowercase;\n
 * classicnames; -> Formats all words as the classical name writing form;\n
 * @return This function returns a Bool value based on Defined Correspondence or default true if no given matches.
 * @return This function can modify a string with the given commands listed above.
 */
_Bool sString (char **getIns, const char words[], const char commands[]) {
    if (strcmp(commands, "NULL") != 0){
        s_Commands_Exec(commands,&(*getIns));
    }
    if (strcmp(words, "NULL") != 0) {
        return AnalyzeWords(words, *getIns);
    }
    return true;
}

_Bool ccLess_Comparison(const char string[], const char string_[]) {
    unsigned int comparison;
    for (int i = 0; string[i] >= ' ' && string_[i] >= ' ' && string[i] <= 'z' && string_[i] <= 'z'; i++) {
        if (string[i] == string_[i]) comparison = 1;
        else if (string[i] >= 'A' && string[i] <= 'Z' && string_[i] >= 'a' && string_[i] <= 'z') {
            if (string[i]+' ' == string_[i]) {
                comparison = 1;
            }
        }
        else if (string[i] >= 'a' && string[i] <= 'z' && string_[i] >= 'A' && string_[i] <= 'Z') {
            if (string[i] == string_[i] + ' ') {
                comparison = 1;
            }
        }
        else return false;
    }
    return comparison;
}

static _Bool ignoreCaps(char _char, char _char2) {
    if ((_char >= 'A' && _char <= 'Z') || (_char >= 'a' && _char <= 'z')) {
        if ((_char2 >= 'A' && _char2 <= 'Z') || (_char2 >= 'a' && _char2 <= 'z')) {
            return (_char == _char2 || (_char - 32) == _char2 || _char  == (_char2 - 32));
        }
    }
    return _char == _char2;
}

static int *containsSupportPos(char *_chars, _Bool caseSensitive) {
    int *insucc = (int*) calloc(strlen(_chars), sizeof(int));
    insucc[0] = 0;
    for (int i = 1, j = 0; i < strlen(_chars); i++) {
        if ((!caseSensitive && ignoreCaps(_chars[j], _chars[i])) || _chars[j] == _chars[i]) {
            j++;
            insucc[i] = j;
        }
        else if (j != 0) {
            j = insucc[j-1];
        }
        else {
            insucc[i] = 0;
        }
    }
    return insucc;
}

match *contains(char *_chars, char *_str, _Bool caseSensitive) {
    match *result = (match*) calloc(1, sizeof(match));
    (*result).success = false;
    (*result).index = 0;
    (*result).indexes = (int*) calloc(1, sizeof(int));
    int *insucc = containsSupportPos(_chars, caseSensitive);
    for (int i = 0, j = 0; i < strlen(_str);) {
        if ((!caseSensitive && ignoreCaps(_str[i], _chars[j])) || _str[i] == _chars[j]) {
            i++;
            j++;
        }
        if (j == strlen(_chars)) {
            (*result).success = true;
            (*result).index++;
            (*result).indexes = (int*) realloc((*result).indexes, (*result).index * sizeof(int));
            (*result).indexes[(*result).index-1] = i-j;
            j = 0;
        }
        else if (i < strlen(_str)) {
            _Bool selection = (!caseSensitive) ? !ignoreCaps(_chars[j], _str[i]) : _chars[j] != _str[i];
            if (selection) {
                if (j != 0) {
                    j = insucc[j - 1];
                } else {
                    i++;
                }
            }
        }
    }

    free(insucc);
    return result;
}

int **checkMatches(match **matches) {
    if ((**matches).success) {
        int **result = (int**) calloc(2, sizeof(int*));
        result[0] = (int*) calloc(1, sizeof(int));
        result[0][0] = (**matches).index;
        result[1] = (int*) calloc(result[0][0], sizeof(int));
        for (int i = 0; i < result[0][0]; i++) {
            result[1][i] = (**matches).indexes[i];
        }
        free((**matches).indexes);
        free(*matches);
        return result;
    }
    free((**matches).indexes);
    free(*matches);
    return NULL;
}

char *numToString(int num) {
    char *numb = (char*) calloc(11, sizeof(char));
    numb[0] = '0';
    int dim = 0;
    while (num != 0) {
        for (int i = dim; dim != 0 && i > -1; i--) {
            numb[i] = numb[i - 1];
        }
        numb[0] = (num % 10) + 48;
        num /= 10;
        dim++;
    }
    numb = (char*) realloc(numb, sizeof(char)*(strlen(numb) + 1));
    return numb;
}

char *charToString(const char _char) {
    static char str[1];
    str[0] = _char;
    return str;
}

void bubbleSortInt(int **array, int dimension) {
    for (int i = 0; i < dimension - 1; i++) {
        for (int j = 0, tmp; j < dimension - i - 1; j++) {
            if ((*array)[j] > (*array)[j+1]) {
                tmp = (*array)[j];
                (*array)[j] = (*array)[j+1];
                (*array)[j+1] = tmp;
            }
        }
    }
}
