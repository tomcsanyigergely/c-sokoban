#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

#include "globals.h"
#include "game.h"
#include "states.h"
#include "drawer.h"

/// RAJZOL�SOKAT V�GZ� MODUL

/**
 * Egy sz�mot sz�vegg� konvert�l.
 * number - a konvert�land� sz�m
 * str - az el��ll�tott sz�veg
 */
void ToString(int number, char *str) {
    int index = 0;
    if (number == 0) {str[0] = '0'; index++;}
    else if (number < 0) {str[0] = '-'; index++; ToString2(-number, str, &index);}
    else ToString2(number, str, &index);
    str[index] = '\0';
}

/**
 * Az el�z� f�ggv�ny seg�df�ggv�nye.
 */
void ToString2(int number, char *str, int *index) {
    if (number != 0) {
        ToString2(number/10, str, index);
        str[*index] = '0' + number%10;
        *index = *index + 1;
    }
}

/**
 * Kirajzol egy list�s men�t.
 * screen - k�perny� c�me
 * menu - a rajzoland� men� strukt�ra c�me
 * offset - az eltol�s m�rt�ke
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
 * Az ir�ny f�ggv�ny�ben visszaadja a megjelen�tend� karakter megfelel� k�p c�m�t.
 * direction - az ir�ny vektora
 * RETURN - a megjelen�tend� k�p c�me
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
 * Kirajzol egy mez�t a p�lyafel�letre.
 * screen - k�perny� c�me
 * field - a kirajzoland� mez�
 * pos - a mez� koordin�t�i
 * direction - a karakter ir�nya
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
 * A szerkeszt�ben megrajzolja a palett�t.
 * screen - k�perny� c�me
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
 * Megrajzolja a p�ly�t a k�perny�re.
 * screen - k�perny� c�me
 * offset - eltol�s m�rt�ke
 * level - a p�lya adatai
 * levelSize - p�lya m�rete
 * direction - karakter ir�nya
 */
void drawLevel(SDL_Surface *screen, Vector offset, Field **level, Vector levelSize, Vector direction){
    int i, j;
    for(i = 0; i < levelSize.y; i++)
    for(j = 0; j < levelSize.x; j++)
        drawField(screen, level[i][j], (Vector){j*FIELD_SIZE+offset.x, i*FIELD_SIZE+offset.y}, direction);

}
