#define MAX_ROUTINES 100
typedef struct {
    char petName[50];
    char exercise[100];
} ExerciseRoutine;

typedef struct {
    ExerciseRoutine stack[MAX_ROUTINES];
    int top;
} ExerciseStack;

ExerciseStack exerciseStack = { exerciseStack.top = -1 };