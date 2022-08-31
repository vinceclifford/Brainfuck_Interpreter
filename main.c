#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#pragma GCC diagnostic ignored "-Wpedantic"


#define DISPATCH goto**(pc++);
#define DISPATCH_PART1 void* nextLabel = *(pc++);
#define DISPATCH_PART2 goto* nextLabel;

// Interpreter state
typedef struct BFState {
    // The array and the size of the array.
    size_t array_len;
    uint8_t* array;

    // Pointer to the current position in the array; array <= cur < (array+array_len)
    uint8_t* cur;
} BFState;

int brainfuck();

int main(int argc, char *argv[]) {
    if(argc != 2){
        fprintf(stderr, "Wrong amount of command line arguments specified");
        exit(EXIT_FAILURE);
    }


    uint8_t* array = malloc(30000 * sizeof (uint8_t));
    BFState* bf = malloc( sizeof (BFState));

    bf->array_len = 30000;
    bf->array = array;
    bf->cur = array;

    brainfuck(bf, argv[1]);

    free(array);
    free(bf);

    return 0;
}

int brainfuck(struct BFState* state, const char* program) {
    void* stack[256];
    void** stackFast = (void**)stack;
    const char** stackSlow = (const char**)stack;
    int counterOfStack = 0;

    const int stackFrameOverhead = 100;
    const int maxBufferSize = (4096 - stackFrameOverhead - sizeof(stack)) / (sizeof(void*) + sizeof(uint16_t));
    uint16_t loopOffset[maxBufferSize];
    int16_t openBrackets = 0;
    int16_t* openBracketOffsetStack = (int16_t*)stack;
    void* directThreadedProgramBase[maxBufferSize];
    void** pc = directThreadedProgramBase;
    uint8_t* constSize = state->array + state->array_len;
    uint8_t* stateCurLocal = state->cur;

    int i = 0;
    {
        while (1) {
            if (i >= maxBufferSize) goto slow_but_small;
            switch (program[i]) {
                case '+': {
                    if (program[i + 1] == '+') {
                        directThreadedProgramBase[i] = && add_value_by_n;
                        int c = i;
                        while (program[i + 1] == '+') {
                            i++;
                        }
                        size_t amount = i - c + 1;
                        void* toAmount = (void*)amount;
                        directThreadedProgramBase[c + 1] = toAmount ;
                    } else {
                        directThreadedProgramBase[i] = &&inc_value;
                    }
                    break;
                }
                case '-': {
                    if (program[i + 1] == '-') {
                        directThreadedProgramBase[i] = && sub_value_by_n;
                        int c = i;
                        while (program[i + 1] == '-') {
                            i++;
                        }
                        size_t amount = i - c + 1;
                        void* toAmount = (void*)amount;
                        directThreadedProgramBase[c + 1] = toAmount ;
                    } else {
                        directThreadedProgramBase[i] = &&dec_value;
                    }
                    break;
                }
                case '>': {
                    directThreadedProgramBase[i] = &&inc_pointer;
                    break;
                }
                case '<': {
                    directThreadedProgramBase[i] = &&dec_pointer;
                    break;
                }
                case '[': {
                    openBracketOffsetStack[openBrackets++] = i;
                    directThreadedProgramBase[i] = &&start_loop;
                    break;
                }
                case ']': {
                    if (openBrackets > 0) {
                        openBrackets--;
                        loopOffset[openBracketOffsetStack[openBrackets]] = i - openBracketOffsetStack[openBrackets];
                    }
                    directThreadedProgramBase[i] = &&end_loop;
                    break;
                }
                case '.': {
                    directThreadedProgramBase[i] = &&pnt;
                    break;
                }
                case '\0': {
                    directThreadedProgramBase[i] = &&end_program;
                    if (openBrackets != 0)
                        return -1;
                    goto fast_direct_threaded;
                }
                default: {
                    directThreadedProgramBase[i] = &&comment;
                    break;
                }
            }
            i++;
        }
    }

    fast_direct_threaded:

    DISPATCH;
    while (1) {
        comment : {
        DISPATCH;
    }

        end_program : {
        state->cur = stateCurLocal;
        return counterOfStack == 0 ? 0 : -1;
    }

        pnt : {
        DISPATCH_PART1;
        printf("%d\n", *stateCurLocal);
        DISPATCH_PART2;
    }

        inc_value : {
        DISPATCH_PART1;
        (*stateCurLocal)++;
        DISPATCH_PART2;
    }
        add_value_by_n : {
        void* nextLabel = *(pc);
        size_t toAdd = (size_t)(nextLabel);
        *stateCurLocal = toAdd + *stateCurLocal;
        pc = pc + toAdd - 1;
        DISPATCH;
    }
        sub_value_by_n : {
        void* nextLabel = *(pc);
        size_t toSub = (size_t)(nextLabel);
        *stateCurLocal = *stateCurLocal - toSub;
        pc = pc + toSub - 1;
        DISPATCH;
    }
        dec_value : {
        DISPATCH_PART1;
        (*stateCurLocal)--;
        DISPATCH_PART2;
    }
        inc_pointer : {
        DISPATCH_PART1;
        stateCurLocal++;
        if (stateCurLocal >= constSize)
            return -1;
        DISPATCH_PART2;
    }
        dec_pointer : {
        DISPATCH_PART1;
        stateCurLocal--;
        if (stateCurLocal < state->array)
            return -1;
        DISPATCH_PART2;
    }
        start_loop : {
        if (*stateCurLocal < 1) {
            pc = pc + loopOffset[pc - directThreadedProgramBase - 1];
        } else {
            stackFast[counterOfStack++] = pc;
        }
        DISPATCH;
    }
        end_loop : {
        if (counterOfStack == 0) {
            if (*stateCurLocal == 0) {
                DISPATCH;
            } else {
                return -1;
            }
        }

        if (*stateCurLocal == 0) {
            counterOfStack--;
        } else {
            pc = stackFast[counterOfStack - 1];
        }
        DISPATCH;
    }
    }
    return -1;

    slow_but_small:

    while (*program != '\0') {
        switch (*program) {
            case '+': {
                uint8_t* arr = stateCurLocal;
                (*arr)++;
                break;
            }
            case '-': {
                uint8_t* arr = stateCurLocal;
                (*arr)--;
                break;
            }
            case '.': {
                printf("%d\n", *stateCurLocal);
                break;
            }
            case '>': {
                stateCurLocal++;
                if (stateCurLocal >= constSize)
                    return -1;
                break;
            }
            case '<': {
                stateCurLocal--;
                if (stateCurLocal < state->array)
                    return -1;
                break;
            }
            case '[': {
                if (*stateCurLocal < 1) {
                    int counter = 1;

                    while (counter > 0) {
                        program++;
                        char lookingAt = *program;
                        if (lookingAt == '\0')
                            return -1;
                        if (lookingAt == '[')
                            counter++;
                        else if (lookingAt == ']')
                            counter--;
                    }
                } else {
                    stackSlow[counterOfStack++] = program;
                }
                break;
            }
            case ']': {
                if (counterOfStack == 0) {
                    if (*stateCurLocal == 0) {
                        break;
                    } else {
                        return -1;
                    }
                }

                if (*stateCurLocal == 0) {
                    counterOfStack--;
                } else {
                    program = stackSlow[counterOfStack - 1];
                }

                break;
            }
        }
        program++;
    }
    return counterOfStack == 0 ? 0 : -1;
}

