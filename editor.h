#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

void count(int*, int*, Field**, Vector);
void autoWall(Field**, Vector);
bool removable(Field**, Vector, Vector);
bool findSelectedField(Field **, Vector, Vector, Vector, Vector *, Field **);
void setCursor(Vector *, SDL_Event *);
bool testHasPlayer(Field **, Vector);
void clearContent(Field *, bool *, int *);
void clearType(Field *, int *);
void clearWall(Field **, Vector);

#endif // EDITOR_H_INCLUDED
