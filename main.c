#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <stdbool.h>
#include <string.h>

#include "globals.h"
#include "game.h"
#include "states.h"
#include "drawer.h"
#include "editor.h"
#include "file.h"

/// FÕ MODUL

int main(int argc, char *argv[]) {
    SDL_Event event; /// esemény tárolása
    SDL_Surface *screen; /// képernyõ
    GameState gamestate = MainMenu; /// játékállapot inicializálása
    GameMode gamemode; /// játékmód
    bool builtInLevels; /// választott pályafajták (belépített vagy nem beépített pályák)
    ResultMode resultmode; /// eredmény kijelzésének módja
    Field** level; /// pálya adatai
    Vector levelSize, startPos; /// pályaméret és kezdõpozíció
    Name playerName; /// játékos neve
    Name levelName; /// pálya neve
    int minStep; /// minimális lépésszám
    bool valid; /// a szerkesztõbe betöltött pálya érvényes-e

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_ANYFORMAT);
    if (!screen) {
        fprintf(stderr, "Nem sikerult megnyitni az ablakot!\n");
        exit(1);
    }
    SDL_WM_SetCaption("Sokoban Demo", "Sokoban Demo");

    while (gamestate != Close) {
        if (event.type == SDL_QUIT) gamestate = Close;
        else switch(gamestate) {
            case MainMenu: mainMenu(screen, &event, &gamestate, &gamemode); break;
            case EditorMenu: editorMenu(screen, &event, &gamestate, &valid); break;
            case CreateLevel: createLevelMenu(screen, &event, &gamestate, &level, &levelSize, &minStep); break;
            case SelectLevel: selectLevel(screen, &event, &level, &levelSize, &startPos, levelName, playerName, &minStep, builtInLevels, &gamestate, gamemode); break;
            case PlayLevel: playLevel(screen, &event, level, levelSize, startPos, &gamestate, gamemode, &valid, &minStep, playerName, levelName); break;
            case EditLevel: editLevel(screen, &event, level, levelSize, &startPos, &gamestate, &valid, &minStep); break;
            case AskInput: askInput(screen, &event, level, levelSize, minStep, &gamestate, gamemode); break;
            case SelectPlayer: selectPlayer(screen, &event, playerName, &gamestate); break;
            case SelectLevelType: selectLevelType(screen, &event, &builtInLevels, &gamestate); break;
            case ResultMenu: resultMenu(screen, &event, &resultmode, &gamestate); break;
            case ResultList: resultList(screen, &event, &gamestate, resultmode); break;
            case Help: help(screen, &event, &gamestate); break;
            default: break;
        }
    }

    SDL_Quit();

    return 0;
}
