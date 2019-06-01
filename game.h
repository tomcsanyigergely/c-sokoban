#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

typedef enum FieldType {
    Empty,
    Wall,
    Road,
    Target
} FieldType;

typedef enum FieldContent {
    None,
    Box,
    Player
} FieldContent;

typedef struct Field {
    FieldType type;
    FieldContent content;
} Field;

typedef struct Vector {
    int x, y;
} Vector;

int min(int, int);
int max(int, int);
bool move(Field**, Vector*, Vector*, Vector);
void mallocLevel(Field***, Vector);
void freeLevel(Field**, Vector);
void resetLevel(Field **, Field ***, Vector);
bool test(Field**, Vector);

#endif // GAME_H_INCLUDED
