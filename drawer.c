#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

#include "globals.h"
#include "game.h"
#include "states.h"
#include "drawer.h"

/// RAJZOLÁSOKAT VÉGZÕ MODUL

/**
 * Egy számot szöveggé konvertál.
 * number - a konvertálandó szám
 * str - az elõállított szöveg
 */
void ToString(int number, char *str) {
    int index = 0;
    if (number == 0) {str[0] = '0'; index++;}
    else if (number < 0) {str[0] = '-'; index++; ToString2(-number, str, &index);}
    else ToString2(number, str, &index);
    str[index] = '\0';
}

/**
 * Az elõzõ függvény segédfüggvénye.
 */
void ToString2(int number, char *str, int *index) {
    if (number != 0) {
        ToString2(number/10, str, index);
        str[*index] = '0' + number%10;
        *index = *index + 1;
    }
}

/**
 * Kirajzol egy listás menüt.
 * screen - képernyõ címe
 * menu - a rajzolandó menü struktúra címe
 * offset - az eltolás mértéke
 */
void drawOptionMenu(SDL_Surface *screen, OptionMenu *menu, int offset) {
    if (menu->choice == menu->optionNumber) menu->choice = 0;
    else if (menu->choice == -1) menu->choice = menu->optionNumber-1;
    boxColor(screen, 0, 0, WIDTH, HEIGHT, BLACK);
    int i;
    for(i = 0; i < menu->optionNumber; i++)
        stringColor(screen, DIST, DIST+(i+offset)*DIST, menu->options[i], (i == menu->choice) ? GREEN : WHITE);
}

/**
 * Az irány függvényében visszaadja a megjelenítendõ karakter megfelelõ kép címét.
 * direction - az irány vektora
 * RETURN - a megjelenítendõ kép címe
 */
SDL_Surface** setPlayerImage(Vector direction) {
    static bool reset = true;
    static SDL_Surface* downImage;
    static SDL_Surface* upImage;
    static SDL_Surface* rightImage;
    static SDL_Surface* leftImage;

    if (reset) {
        downImage = IMG_Load("data/images/down.png");
        upImage = IMG_Load("data/images/up.png");
        rightImage = IMG_Load("data/images/right.png");
        leftImage = IMG_Load("data/images/left.png");
        reset = false;
    }

    if (direction.y == 1) return &downImage;
    else if (direction.y == -1) return &upImage;
    else if (direction.x == 1) return &rightImage;
    else if (direction.x == -1) return &leftImage;
    else return NULL;
}

/**
 * Kirajzol egy mezõt a pályafelületre.
 * screen - képernyõ címe
 * field - a kirajzolandó mezõ
 * pos - a mezõ koordinátái
 * direction - a karakter iránya
 */
void drawField(SDL_Surface *screen, Field field, Vector pos, Vector direction) {
    static bool reset = true;
    static SDL_Surface* emptyImage;
    static SDL_Surface* wallImage;
    static SDL_Surface* roadImage;
    static SDL_Surface* boxImage;
    static SDL_Surface* box2Image;
    static SDL_Surface* targetImage;

    if (reset) {
        emptyImage = IMG_Load("data/images/empty.png");
        wallImage = IMG_Load("data/images/wall.png");
        roadImage = IMG_Load("data/images/road.png");
        boxImage = IMG_Load("data/images/box.png");
        box2Image = IMG_Load("data/images/box2.png");
        targetImage = IMG_Load("data/images/target.png");
        reset = false;
    }

    SDL_Surface** image;

    switch(field.type) {
        case Empty: image = &emptyImage; break;
        case Wall:  image = &wallImage;  break;
        case Road:
            switch(field.content) {
                case None: image = &roadImage; break;
                case Box:  image = &boxImage;  break;
                case Player: image = setPlayerImage(direction); break;
            } break;
        case Target:
            switch(field.content) {
                case None: image = &targetImage; break;
                case Box:  image = &box2Image; break;
                case Player: image = setPlayerImage(direction); break;
            } break;
    }

    SDL_Rect sourceImage = {0, 0, FIELD_SIZE, FIELD_SIZE};
    SDL_Rect targetArea = {pos.x, pos.y, FIELD_SIZE, FIELD_SIZE};
    SDL_BlitSurface(*image, &sourceImage, screen, &targetArea);
}

/**
 * A szerkesztõben megrajzolja a palettát.
 * screen - képernyõ címe
 */
void drawPalett(SDL_Surface *screen){
    boxColor(screen, WIDTH-5*FIELD_SIZE, 0, WIDTH, HEIGHT, DARKGREEN);
    drawField(screen, (Field){Wall, None}, (Vector){WIDTH-4.5*FIELD_SIZE, 1*FIELD_SIZE}, (Vector){0, 1});
    drawField(screen, (Field){Road, None}, (Vector){WIDTH-3*FIELD_SIZE, 1*FIELD_SIZE}, (Vector){0, 1});
    drawField(screen, (Field){Target, None}, (Vector){WIDTH-1.5*FIELD_SIZE, 1*FIELD_SIZE}, (Vector){0, 1});
    drawField(screen, (Field){Road, Box}, (Vector){WIDTH-4.5*FIELD_SIZE, 2.5*FIELD_SIZE}, (Vector){0, 1});
    drawField(screen, (Field){Road, Player}, (Vector){WIDTH-3*FIELD_SIZE, 2.5*FIELD_SIZE}, (Vector){0, 1});
}

/**
 * Megrajzolja a pályát a képernyõre.
 * screen - képernyõ címe
 * offset - eltolás mértéke
 * level - a pálya adatai
 * levelSize - pálya mérete
 * direction - karakter iránya
 */
void drawLevel(SDL_Surface *screen, Vector offset, Field **level, Vector levelSize, Vector direction){
    int i, j;
    for(i = 0; i < levelSize.y; i++)
    for(j = 0; j < levelSize.x; j++)
        drawField(screen, level[i][j], (Vector){j*FIELD_SIZE+offset.x, i*FIELD_SIZE+offset.y}, direction);

}
