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

/// F� MODUL

int main(int argc, char *argv[]) {
    SDL_Event event; /// esem�ny t�rol�sa
    SDL_Surface *screen; /// k�perny�
    GameState gamestate = MainMenu; /// j�t�k�llapot inicializ�l�sa
    GameMode gamemode; /// j�t�km�d
    bool builtInLevels; /// v�lasztott p�lyafajt�k (bel�p�tett vagy nem be�p�tett p�ly�k)
    ResultMode resultmode; /// eredm�ny kijelz�s�nek m�dja
    Field** level; /// p�lya adatai
    Vector levelSize, startPos; /// p�lyam�ret �s kezd�poz�ci�
    Name playerName; /// j�t�kos neve
    Name levelName; /// p�lya neve
    int minStep; /// minim�lis l�p�ssz�m
    bool valid; /// a szerkeszt�be bet�lt�tt p�lya �rv�nyes-e

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
