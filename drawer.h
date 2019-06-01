#ifndef DRAWER_H_INCLUDED
#define DRAWER_H_INCLUDED

void ToString(int, char*);
void ToString2(int, char*, int*);
void drawOptionMenu(SDL_Surface*, OptionMenu*, int);
SDL_Surface** setPlayerImage(Vector);
void drawField(SDL_Surface*, Field, Vector, Vector);
void drawPalett(SDL_Surface*);
void drawLevel(SDL_Surface*, Vector, Field**, Vector, Vector);

#endif // DRAWER_H_INCLUDED
