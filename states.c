#include <SDL.h>
#include <stdbool.h>

#include "globals.h"
#include "game.h"
#include "states.h"
#include "file.h"

/// A JÁTÉK ÁLLAPOTAINAK MODULJA

/**
 * Létrehoz egy megadott opciókat tartalmazó listamenüt.
 * optionNumber - opciók száma
 * options - opciókat tartalmazó "tömb"
 * RETURN - az elkészített listamenü-struktúra
 */
OptionMenu newOptionMenu(int optionNumber, Option *options) {
    OptionMenu menu;
    menu.optionNumber = optionNumber;
    menu.options = (Option*)malloc(optionNumber*sizeof(Option));
    int i;
    for(i = 0; i < optionNumber; i++) strcpy(menu.options[i], options[i]);
    return menu;
}

/**
 * FÕMENÜ
 * screen - képernyõ memóriacíme
 * event - esemény
 * gamestate - játékállapot
 * gamemode - játékmód
 */
void mainMenu(SDL_Surface *screen, SDL_Event *event, GameState *gamestate, GameMode *gamemode) {
    static bool reset = true, init = true, changed;
    static OptionMenu menu;

    ///RESET
    if (reset) {
        menu = newOptionMenu(5, (Option[]){{"New Game"}, {"Level Editor"}, {"Results"}, {"Help"}, {"Exit"} });
        reset = false;
    }

    ///INIT
    if (init) {
        menu.choice = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        drawOptionMenu(screen, &menu, 0);
        stringColor(screen, DIST, HEIGHT-2*DIST, "Gergely Tomcsanyi, 2016.12.04.", ORANGE);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym) {
                case SDLK_DOWN: menu.choice++; changed = true; break;
                case SDLK_UP:   menu.choice--; changed = true; break;
                case SDLK_ESCAPE: *gamestate = Close; break;
                case SDLK_RETURN: switch(menu.choice) {
                        case 0: *gamestate = SelectPlayer; *gamemode = Play; init = true; break;
                        case 1: *gamestate = EditorMenu; *gamemode = Editor; init = true; break;
                        case 2: *gamestate = ResultMenu; init = true; break;
                        case 3: *gamestate = Help; init = true; break;
                        case 4: *gamestate = Close; break;
                        default: break;
                } break;
                default: break;
            } break;
    }
}

/**
 * JÁTÉKOS-PROFIL VÁLASZTÁSA
 * screen - képernyõ címe
 * event - esemény
 * playerName - ide kerül a kiválasztott játékos neve
 * gamestate - játékállapot
 */
void selectPlayer(SDL_Surface *screen, SDL_Event *event, char *playerName, GameState *gamestate){
    static bool reset = true, init = true, changed;
    static int offset;
    static OptionMenu menu;

    ///RESET
    if (reset) {
        menu.options = NULL;
        reset = false;
    }

    ///INIT
    if (init) {
        free(menu.options);
        menu = playerList();
        offset = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        drawOptionMenu(screen, &menu, offset);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN: switch(event->key.keysym.sym) {
                            case SDLK_DOWN: menu.choice++; changed = true; break;
                            case SDLK_UP: menu.choice--; changed = true; break;
                            case SDLK_ESCAPE: *gamestate = MainMenu; init = true; break;
                            case SDLK_RETURN:   if (menu.choice == 0) *gamestate = AskInput;
                                                else if (menu.choice == menu.optionNumber-1) *gamestate = MainMenu;
                                                else {
                                                    strcpy(playerName, menu.options[menu.choice]);
                                                    *gamestate = SelectLevelType;
                                                }
                                                init = true; break;
                            default: break;
                            } break;
        case SDL_MOUSEBUTTONUP: switch(event->button.button) {
                                    case SDL_BUTTON_WHEELUP: if (offset < 0) offset += 1; changed = true; break;
                                    case SDL_BUTTON_WHEELDOWN: offset -= 1; changed = true; break;
                                    default: break;
                                } break;
    }
}

/**
 * PÁLYATÍPUS VÁLASZTÁSA
 * screen - képernyõ címe
 * event - esemény
 * builtInLevels - ide kerül, hogy a játékos beépített pályákkal fog-e játszani
 * gamestate - játékállapot
 */
void selectLevelType(SDL_Surface *screen, SDL_Event *event, bool *builtInLevels, GameState *gamestate) {
    static bool init = true, reset = true, changed;
    static OptionMenu menu;

    ///RESET
    if (reset) {
        menu = newOptionMenu(3, (Option[]){{"Standard Mode"},{"Casual Mode"},{"Quit Profile"}});
        reset = false;
    }

    ///INIT
    if (init) {
        menu.choice = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        drawOptionMenu(screen, &menu, 0);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN:   switch(event->key.keysym.sym) {
                                case SDLK_DOWN: menu.choice++; changed = true; break;
                                case SDLK_UP:   menu.choice--; changed = true; break;
                                case SDLK_ESCAPE: *gamestate = SelectPlayer; init = true; break;
                                case SDLK_RETURN:   switch(menu.choice) {
                                                        case 0: *builtInLevels = true; *gamestate = SelectLevel; break;
                                                        case 1: *builtInLevels = false; *gamestate = SelectLevel; break;
                                                        case 2: *gamestate = SelectPlayer; break;
                                                    } init = true; break;
                                default: break;
                            } break;
    }
}

/**
 * PÁLYA VÁLASZTÁSA
 * screen - képernyõ címe
 * event - esemény
 * level - ide kerülnek a választott pálya adatai
 * levelSize - ide kerül a betöltött pálya mérete
 * startPos - ide kerül a karakter kezdõpozíciója
 * levelName - ide kerül a kiválasztott pálya neve
 * playerName - játékos neve
 * minStep - ide kerül a minimális lépésszám
 * builtInLevels - értéke logikai igaz, ha a játékos beépített pályákkal játszik
 * gamestate - játék állapot
 * gamemode - játékmód
 */
void selectLevel(SDL_Surface *screen, SDL_Event *event, Field ***level, Vector *levelSize, Vector *startPos, char *levelName, char *playerName, int *minStep, bool builtInLevels, GameState *gamestate, GameMode gamemode) {
    static bool reset = true, init = true, changed;
    static int offset;
    static OptionMenu menu;

    ///RESET
    if (reset) {
        menu.options = NULL;
        reset = false;
    }

    ///INIT
    if (init) {
        free(menu.options);
        menu = levelList(playerName, builtInLevels, gamemode);
        offset = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        drawOptionMenu(screen, &menu, offset);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN: switch(event->key.keysym.sym){
                                case SDLK_DOWN: menu.choice++; changed = true; break;
                                case SDLK_UP: menu.choice--; changed = true; break;
                                case SDLK_RETURN:   if (menu.choice == menu.optionNumber-1) {switch(gamemode) {
                                                        case Editor: *gamestate = EditorMenu; break;
                                                        case Play: *gamestate = SelectLevelType; break;
                                                    } init = true;}
                                                    else {
                                                        *minStep = minStepOfLevel(menu.options[menu.choice]);
                                                        strcpy(levelName, menu.options[menu.choice]);
                                                        loadLevel(level, levelSize, startPos, menu.options[menu.choice]);
                                                        switch(gamemode) {
                                                            case Editor: *gamestate = EditLevel; break;
                                                            case Play: *gamestate = PlayLevel; break;
                                                        }
                                                    } init = true; break;
                                case SDLK_ESCAPE: switch(gamemode) {
                                                    case Editor: *gamestate = EditorMenu; break;
                                                    case Play: *gamestate = SelectLevelType; break;
                                                    } init = true; break;
                                default: break;
                           } break;
        case SDL_MOUSEBUTTONUP: switch(event->button.button) {
                                    case SDL_BUTTON_WHEELUP: if (offset < 0) offset += 1; changed = true; break;
                                    case SDL_BUTTON_WHEELDOWN: offset -= 1; changed = true; break;
                                    default: break;
        }
    }
}

/**
 * JÁTÉK ÁLLAPOT
 * screen - képernyõ címe
 * event - esemény
 * initial - pálya kezdeti állapotának adatai
 * levelSize - pálya mérete
 * startPos - kezdõpozíció
 * gamestate - játékállapot
 * gamemode - játékmód
 * valid - értéke igaz, ha a pálya teljesítve van
 * minStep - ide kerül a minimális lépésszám
 * playerName - játékos neve
 * levelName - pálya neve
 */
void playLevel(SDL_Surface *screen, SDL_Event *event, Field **initial, Vector levelSize, Vector startPos, GameState *gamestate, GameMode gamemode, bool *valid, int *minStep, char* playerName, char* levelName){
    static bool init = true, changed, tracking, levelIsValid;
    static Field** level = NULL;
    static Vector pos, offset, direction;
    static int step;

    ///INIT
    if (init) {
        resetLevel(initial, &level, levelSize);
        pos = startPos;
        offset = (Vector){(WIDTH/2/FIELD_SIZE-pos.x)*FIELD_SIZE, (HEIGHT/2/FIELD_SIZE-pos.y)*FIELD_SIZE};
        direction = (Vector){0, 1};
        changed = true;
        tracking = true;
        levelIsValid = false;
        step = 0;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        if (!levelIsValid) {
            boxColor(screen, 0, 0, WIDTH, HEIGHT, BLACK);
            drawLevel(screen, offset, level, levelSize, direction);
            NumberStr StepStr;
            ToString(step, StepStr);
            stringColor(screen, DIST, DIST, StepStr, WHITE);
            ToString(*minStep, StepStr);
            stringColor(screen, DIST, 2*DIST, (gamemode == Editor && *valid == false) ? "N/A" : StepStr, YELLOW);
        }
        else {
            Message msg = "Level completed! (";
            NumberStr StepStr;
            ToString(step, StepStr);
            strcat(msg, StepStr);
            strcat(msg, " steps)");
            boxColor(screen, (WIDTH-DIALOG_WIDTH)/2, (HEIGHT-DIALOG_HEIGHT)/2, (WIDTH+DIALOG_WIDTH)/2, (HEIGHT+DIALOG_HEIGHT)/2, DARKGREEN);
            stringColor(screen, (WIDTH-DIALOG_WIDTH)/2, (HEIGHT-DIALOG_HEIGHT)/2, msg, GREEN);
            stringColor(screen, (WIDTH-DIALOG_WIDTH)/2, (HEIGHT-DIALOG_HEIGHT)/2+DIST, "Press Enter to continue.", ORANGE);
        }
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    bool motion = false;
    if (!levelIsValid) {
        switch (event->type) {
                case SDL_KEYDOWN:
                    switch(event->key.keysym.sym) {
                        case SDLK_a:    offset.x += FIELD_SIZE; tracking = false; changed = true; break;
                        case SDLK_d:    offset.x -= FIELD_SIZE; tracking = false; changed = true; break;
                        case SDLK_w:    offset.y += FIELD_SIZE; tracking = false; changed = true; break;
                        case SDLK_s:    offset.y -= FIELD_SIZE; tracking = false; changed = true; break;

                        case SDLK_LEFT: direction = (Vector){-1, 0}; motion = true; changed = true; break;
                        case SDLK_RIGHT:direction = (Vector){ 1, 0}; motion = true; changed = true; break;
                        case SDLK_UP:   direction = (Vector){ 0,-1}; motion = true; changed = true; break;
                        case SDLK_DOWN: direction = (Vector){ 0, 1}; motion = true; changed = true; break;

                        case SDLK_k:    if (tracking) tracking = false;
                                        else {
                                            tracking = true;
                                            offset = (Vector){(WIDTH/2/FIELD_SIZE-pos.x)*FIELD_SIZE, (HEIGHT/2/FIELD_SIZE-pos.y)*FIELD_SIZE};
                                            changed = true;
                                        } break;
                        case SDLK_r:    init = true;
                                        break;
                        case SDLK_ESCAPE:   freeLevel(level, levelSize);
                                            switch(gamemode) {
                                                case Play: *gamestate = SelectLevel; freeLevel(initial, levelSize); break;
                                                case Editor: *gamestate = EditLevel; break;
                                            }
                                            init = true;
                                            break;
                        default: break;
                    }
                    if (motion && move(level, &pos, &offset, direction)) {
                        if (!levelIsValid) step++;
                        if (tracking) {
                            offset.x -= direction.x*FIELD_SIZE;
                            offset.y -= direction.y*FIELD_SIZE;
                        }
                    }
                    break;
                case SDL_KEYUP: levelIsValid = test(level, levelSize);
                                if(levelIsValid) {
                                        switch(gamemode) {
                                            case Play: saveResult(levelName, playerName, step, *minStep); break;
                                            case Editor: *minStep = *valid ? min(*minStep, step) : step; *valid = *valid || levelIsValid; break;
                                        }
                                    changed = true;
                                }
                                break;
        }
    }
    else if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_RETURN) switch(gamemode) {
        case Editor: *gamestate = EditLevel; init = true; break;
        case Play: *gamestate = SelectLevel; init = true; break;
    }
}

/**
 * SZÖVEG KÉRÉSE A FELHASZNÁLÓTÓL
 * screen - képernyõ címe
 * event - esemény
 * level - pálya adatai
 * levelSize - pálya mérete
 * minStep - legkevesebb lépésszám
 * gamestate - játékállapot
 * gamemode - játékmód
 */
void askInput(SDL_Surface *screen, SDL_Event *event, Field **level, Vector levelSize, int minStep, GameState *gamestate, GameMode gamemode) {
    static bool init = true, changed, overwrite;
    static Name name;
    static Message msg;
    SDL_EnableUNICODE(1);

    ///INIT
    if (init) {
        name[0] = '\0';
        msg[0] = '\0';
        overwrite = false;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        boxColor(screen, (WIDTH-DIALOG_WIDTH)/2, (HEIGHT-DIALOG_HEIGHT)/2, (WIDTH+DIALOG_WIDTH)/2, (HEIGHT+DIALOG_HEIGHT)/2, DARKGREEN);
        boxColor(screen, (WIDTH-INPUT_WIDTH)/2, (HEIGHT-INPUT_HEIGHT)/2, (WIDTH+INPUT_WIDTH)/2, (HEIGHT+INPUT_HEIGHT)/2, GREEN);
        stringColor(screen, (WIDTH-INPUT_WIDTH+DIST)/2, (HEIGHT-INPUT_HEIGHT+DIST)/2, name, DARKGREEN);
        stringColor(screen, (WIDTH-INPUT_WIDTH)/2, (HEIGHT+INPUT_HEIGHT+DIST)/2, msg, ORANGE);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN: { Uint16 uni = event->key.keysym.unicode;
                            if (strlen(name) < NAME_LENGTH && (uni >= (Uint16)'a' && uni <= (Uint16)'z' || uni >= (Uint16)'A' && uni <= (Uint16)'Z'
                            || uni == (Uint16)'_' || uni >= (Uint16)'0' && uni <= (Uint16)'9' || uni == (Uint16)'.')) {
                                strcat(name, (char[]){(char)uni, '\0'});
                                changed = true;
                            }
                            else switch(event->key.keysym.sym) {
                                case SDLK_BACKSPACE: name[strlen(name)-1] = '\0'; changed = true; break;
                                case SDLK_ESCAPE:   switch(gamemode) {
                                                        case Editor: *gamestate = EditLevel; break;
                                                        case Play:   *gamestate = SelectPlayer; break;
                                                        } init = true; break;
                                case SDLK_RETURN:  switch(gamemode) {
                                                        case Editor:
                                                            if (strlen(name) > 0) {
                                                                if (builtIn(name)) {
                                                                    strcpy(msg, "Cannot overwrite a built-in level!");
                                                                }
                                                                else {
                                                                    bool used = nameUsed(name, "data/levels.txt");
                                                                    if (!used || used && overwrite) {
                                                                        saveLevel(level, levelSize, name, minStep, overwrite);
                                                                        strcpy(msg, overwrite ? "Overwrite successful!" : "Save successful!");
                                                                        overwrite = false;
                                                                        name[0] = '\0';
                                                                    }
                                                                    else {
                                                                        strcpy(msg, "Level name already exists. Overwrite it?");
                                                                        overwrite = true;
                                                                    }
                                                                }
                                                            }
                                                            else strcpy(msg, "Invalid name");
                                                            break;
                                                        case Play:
                                                            if (strlen(name) > 0) {
                                                                bool used = nameUsed(name, "data/players.txt");
                                                                if (!used) {
                                                                    savePlayer(name, false);
                                                                    strcpy(msg, "Profile successfully created!");
                                                                    name[0] = '\0';
                                                                }
                                                                else {
                                                                    strcpy(msg, "Player name already exists.");
                                                                    overwrite = true;
                                                                }
                                                            }
                                                            else strcpy(msg, "Invalid name");
                                                            break;
                                                    } changed = true; break;
                                default: break;
                            }
                            } break;
        default: break;
    }
}

/**
 * PÁLYASZERKESZTÕ MENÜ
 * screen - képernyõ címe
 * event - esemény
 * gamestate - játékállapot
 * valid - ide kerül, hogy a betöltött/létrehozott pálya alapból érvényes-e
 */
void editorMenu(SDL_Surface *screen, SDL_Event *event, GameState *gamestate, bool *valid) {
    static bool reset = true, init = true, changed;
    static OptionMenu menu;

    ///RESET
    if (reset) {
        menu = newOptionMenu(3, (Option[]){{"Create Level"}, {"Load Level"}, {"Back to Main Menu"}});
        reset = false;
    }

    ///INIT
    if (init) {
        menu.choice = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        drawOptionMenu(screen, &menu, 0);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym) {
                case SDLK_DOWN: menu.choice++; changed = true; break;
                case SDLK_UP:   menu.choice--; changed = true; break;
                case SDLK_ESCAPE: *gamestate = MainMenu; init = true; break;
                case SDLK_RETURN: switch(menu.choice) {
                    case 0: *valid = false; *gamestate = CreateLevel; init = true; break;
                    case 1: *valid = true;  *gamestate = SelectLevel; init = true; break;
                    case 2: *gamestate = MainMenu; init = true; break;
                } break;
                default: break;
            } break;
    }
}

/**
 * ÚJ PÁLYA MÉRETÉNEK BEÁLLÍTÁSA
 * screen - képernyõ címe
 * event - esemény
 * gamestate - játékállapot
 * level - ide kerül a létrehozott üres pálya
 * levelSize - ide kerül a létrehozott üres pálya mérete
 * minStep - ide kerül a minimális lépésszám
 */
void createLevelMenu(SDL_Surface *screen, SDL_Event *event, GameState *gamestate, Field ***level, Vector *levelSize, int *minStep) {
    static bool reset = true, init = true, changed;
    static OptionMenu menu;
    static int rows, columns;

    ///RESET
    if (reset) {
        menu = newOptionMenu(4, (Option[]){ {"Create"}, {"Rows:"}, {"Columns:"}, {"Cancel"} });
        reset = false;
    }

    ///INIT
    if (init) {
        menu.choice = 0;
        rows = 10;
        columns = 10;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        drawOptionMenu(screen, &menu, 0);
        NumberStr numStr;
        ToString(rows, numStr);
        stringColor(screen, 100, 2*DIST, numStr, (menu.choice == 1) ? GREEN : WHITE);
        ToString(columns, numStr);
        stringColor(screen, 100, 3*DIST, numStr, (menu.choice == 2) ? GREEN : WHITE);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym) {
                case SDLK_DOWN: menu.choice++; changed = true; break;
                case SDLK_UP:   menu.choice--; changed = true; break;
                case SDLK_RIGHT:    switch(menu.choice) {
                                        case 1: if (rows < 100) rows++; changed = true; break;
                                        case 2: if (columns < 100) columns++; changed = true; break;
                                        default: break;
                                    } break;
                case SDLK_LEFT:     switch(menu.choice) {
                                        case 1: if (rows > 5) rows--; changed = true; break;
                                        case 2: if (columns > 5) columns--; changed = true; break;
                                        default: break;
                                    } break;
                case SDLK_ESCAPE: *gamestate = EditorMenu; init = true; break;
                case SDLK_RETURN: switch(menu.choice) {
                    case 0: *levelSize = (Vector){columns, rows};
                            mallocLevel(level, *levelSize); int i, j;
                            for(i = 0; i < rows; i++)
                                for(j = 0; j < columns; j++) {
                                    (*level)[i][j].type = Empty;
                                    (*level)[i][j].content = None;
                            }
                            *minStep = 0;
                            *gamestate = EditLevel; init = true; break;
                    case 3: *gamestate = EditorMenu; init = true; break;
                    default: break;
                } break;
                default: break;
            } break;
    }
}

/**
 * PÁLYA SZERKESZTÉSE
 * screen - képernyõ címe
 * event - esemény
 * level - pálya adatai
 * levelSize - pálya mérete
 * startPos - ez mindig a karakter kezdõpozícióját tartalmazza
 * gamestate - játékállapot
 * valid - értéke logikai igaz, ha a pálya már érvényes és menthetõ
 * minStep - a legkevesebb lépésszámot tárolja
 */
void editLevel(SDL_Surface *screen, SDL_Event *event, Field **level, Vector levelSize, Vector *startPos, GameState *gamestate, bool *valid, int *minStep) {
    static bool init = true, changed, drag, hasPlayer;
    static Field dragField;
    static Field* selected;
    static Vector offset, cursor, selectedPos;
    static int boxCount, targetCount;

    ///INIT
    if (init) {
        offset = (Vector){0, 0};
        changed = true;
        drag = false;
        hasPlayer = testHasPlayer(level, levelSize);
        count(&boxCount, &targetCount, level, levelSize);
        init = false;
    }

    ///OUTPUT
    if (changed) {
        boxColor(screen, 0, 0, WIDTH, HEIGHT, GREY);
        drawLevel(screen, offset, level, levelSize, (Vector){0, 1});
        drawPalett(screen);
        if (drag) drawField(screen, dragField, (Vector){cursor.x-FIELD_SIZE/2, cursor.y-FIELD_SIZE/2}, (Vector){0, 1});
        NumberStr numStr;
        ToString(boxCount, numStr);
        stringColor(screen, DIST, DIST, numStr, WHITE);
        ToString(targetCount, numStr);
        stringColor(screen, DIST, 2*DIST, numStr, WHITE);
        stringColor(screen, DIST, 3*DIST, hasPlayer ? "Igen" : "Nem", WHITE);
        ToString(*minStep, numStr);
        stringColor(screen, DIST, 4*DIST, *valid ? numStr : "Nincs", WHITE);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN: switch(event->key.keysym.sym) {
                            case SDLK_a:    offset.x += FIELD_SIZE; changed = true; break;
                            case SDLK_d:    offset.x -= FIELD_SIZE; changed = true; break;
                            case SDLK_w:    offset.y += FIELD_SIZE; changed = true; break;
                            case SDLK_s:    offset.y -= FIELD_SIZE; changed = true; break;
                            case SDLK_m:    if (*valid) *gamestate = AskInput; changed = true; break;
                            case SDLK_t:    clearWall(level, levelSize); changed = true; break;
                            case SDLK_ESCAPE: freeLevel(level, levelSize); *gamestate = EditorMenu; init = true; break;
                            case SDLK_RETURN: if (hasPlayer && boxCount == targetCount && boxCount > 0) {*gamestate = PlayLevel; init = true;} break;
                            default: break;
                        } break;
        case SDL_MOUSEBUTTONDOWN: {
            if (event->button.button == SDL_BUTTON_LEFT) {
                setCursor(&cursor, event);
                if (cursor.x >= WIDTH-5*FIELD_SIZE) {
                    if (cursor.x >= WIDTH-4.5*FIELD_SIZE && cursor.x < WIDTH-3.5*FIELD_SIZE && cursor.y >= 1*FIELD_SIZE && cursor.y < 2*FIELD_SIZE) { dragField = (Field){Wall, None}; drag = true; changed = true;}
                    if (cursor.x >= WIDTH-3*FIELD_SIZE && cursor.x < WIDTH-2*FIELD_SIZE && cursor.y >= 1*FIELD_SIZE && cursor.y < 2*FIELD_SIZE) { dragField = (Field){Road, None}; drag = true; changed = true;}
                    if (cursor.x >= WIDTH-1.5*FIELD_SIZE && cursor.x < WIDTH-0.5*FIELD_SIZE && cursor.y >= 1*FIELD_SIZE && cursor.y < 2*FIELD_SIZE) { dragField = (Field){Target, None}; drag = true; changed = true;}
                    if (cursor.x >= WIDTH-4.5*FIELD_SIZE && cursor.x < WIDTH-3.5*FIELD_SIZE && cursor.y >= 2.5*FIELD_SIZE && cursor.y < 3.5*FIELD_SIZE) { dragField = (Field){Road, Box}; drag = true; changed = true;}
                    if (cursor.x >= WIDTH-3*FIELD_SIZE && cursor.x < WIDTH-2*FIELD_SIZE && cursor.y >= 2.5*FIELD_SIZE && cursor.y < 3.5*FIELD_SIZE && !hasPlayer) { dragField = (Field){Road, Player}; drag = true; changed = true;}
                }
                else {
                    if(findSelectedField(level, cursor, offset, levelSize, &selectedPos, &selected)) {
                        if (selected->type != Empty) {
                            if (selected->content != None) {clearContent(selected, &hasPlayer, &boxCount); *valid = false; *minStep = 0;}
                            else if (selected->type == Wall && removable(level, selectedPos, levelSize)) clearType(selected, &targetCount);
                            else if (selected->type != Wall) {
                                *valid = false; *minStep = 0;
                                clearType(selected, &targetCount);
                                if (!removable(level, selectedPos, levelSize)) selected->type = Wall;
                            }
                            changed = true;
                        }
                    }
                }
            }
        } break;
        case SDL_MOUSEMOTION: if (drag) {setCursor(&cursor, event); changed = true;} break;
        case SDL_MOUSEBUTTONUP: if (drag) {
            setCursor(&cursor, event);
            if (cursor.x < WIDTH-5*FIELD_SIZE) {
                if (findSelectedField(level, cursor, offset, levelSize, &selectedPos, &selected)) {
                    *valid = false;
                    if (dragField.content != None && selected->content == None && (selected->type == Road || selected->type == Target)) {
                        switch(dragField.content) {
                            case Player: *startPos = selectedPos; hasPlayer = true; break;
                            case Box: boxCount++; break;
                        }
                        selected->content = dragField.content;
                    }
                    else if (dragField.type == Wall && selected->type != Wall) {
                        if (selected->type != Empty) {
                            if (selected->content != None) clearContent(selected, &hasPlayer, &boxCount);
                            clearType(selected, &targetCount);
                        }
                        selected->type = Wall;
                    }
                    else if (dragField.content == None && (dragField.type == Road || dragField.type == Target) && selected->type != dragField.type
                             && selectedPos.x != 0 && selectedPos.x != levelSize.x-1 && selectedPos.y != 0 && selectedPos.y != levelSize.y-1) {
                        if (selected->type != Empty) clearType(selected, &targetCount);
                        if (dragField.type == Target) targetCount++;
                        selected->type = dragField.type;
                        autoWall(level, selectedPos);
                    }
                }
            }
            drag = false;
            changed = true;
            } break;
    }
}

/**
 * EREDMÉNY MENÜ
 * screen - képernyõ címe
 * event - esemény
 * resultmode - értéke [Players] lesz, ha a legjobb játékosokat kívánjuk megtekinteni, míg [Levels] érték esetén
 * azt fogjuk megtekinteni, hogy az egyes pályákat kik teljesítették a legkevesebb lépésszámból
 * gamestate - játékállapot
 */
void resultMenu(SDL_Surface *screen, SDL_Event *event, ResultMode* resultmode, GameState *gamestate) {
    static bool init = true, reset = true, changed;
    static OptionMenu menu;

    ///RESET
    if (reset) {
        menu = newOptionMenu(3, (Option[]){{"Top Players"},{"Top Solvers"},{"Back to Main Menu"}});
        reset = false;
    }

    ///INIT
    if (init) {
        menu.choice = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        drawOptionMenu(screen, &menu, 0);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN:   switch(event->key.keysym.sym) {
                                case SDLK_DOWN: menu.choice++; changed = true; break;
                                case SDLK_UP:   menu.choice--; changed = true; break;
                                case SDLK_ESCAPE: *gamestate = MainMenu; init = true; break;
                                case SDLK_RETURN:   switch(menu.choice) {
                                                        case 0: *resultmode = Players; *gamestate = ResultList; break;
                                                        case 1: *resultmode = Levels; *gamestate = ResultList; break;
                                                        case 2: *gamestate = MainMenu; break;
                                                    } init = true; break;
                                default: break;
                            } break;
    }
}

/**
 * EREDMÉNY KILISTÁZÁSA
 * screen - képernyõ címe
 * event - esemény
 * gamestate - játékállapot
 * resultmode - a választott eredménylista
 */
void resultList(SDL_Surface *screen, SDL_Event *event, GameState *gamestate, ResultMode resultmode) {
    static bool init = true, changed;
    static int offset;

    ///INIT
    if (init) {
        offset = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        boxColor(screen, 0, 0, WIDTH, HEIGHT, BLACK);
        if (resultmode == Players) {
            ///READ DATA
            FILE* fp = fopen("data/players.txt", "r");
            int playerNumber;
            fscanf(fp, " %d", &playerNumber);
            Data* players = (Data*)malloc(playerNumber*sizeof(Data));
            int i;
            for(i = 0; i < playerNumber; i++) fscanf(fp, " %s %d", players[i].name, &players[i].value);
            fclose(fp);

            ///SORT DATA
            int j;
            for(i = 0; i < playerNumber-1; i++)
            for(j = i+1; j < playerNumber; j++)
            if (players[i].value < players[j].value) {
                Data help = players[i];
                players[i] = players[j];
                players[j] = help;
            }

            ///WRITE DATA
            for(i = 0; i < playerNumber; i++) {
                NumberStr numStr;
                ToString(players[i].value, numStr);
                stringColor(screen, DIST, DIST+(i+offset)*DIST, numStr, (i == 0) ? ORANGE : WHITE);
                stringColor(screen, 2*DIST, DIST+(i+offset)*DIST, players[i].name, (i == 0) ? ORANGE : WHITE);
            }
            free(players);
        }
        else if (resultmode == Levels) {
            ///READ DATA
            FILE* fp = fopen("data/levels.txt", "r");
            int levelNumber;
            fscanf(fp, " %d", &levelNumber);
            int row = offset;
            int i;
            for(i = 0; i < levelNumber; i++) {
                Data level;
                fscanf(fp, " %s %d", level.name, &level.value);
                if (builtIn(level.name)) {
                    NumberStr numStr;
                    ToString(level.value, numStr);

                    Message msg;
                    strcpy(msg, level.name);
                    strcat(msg, ": ");

                    FileName file;
                    strcpy(file, "data/results/");
                    strcat(file, level.name);
                    strcat(file, ".txt");
                    FILE* fp2 = fopen(file, "r");
                    int playerNumber;
                    fscanf(fp2, " %d", &playerNumber);
                    int j;
                    bool wrote = false;
                    for(j = 0; j < playerNumber; j++) {
                        Data player;
                        fscanf(fp2, " %s %d", player.name, &player.value);
                        if (player.value == level.value) {
                            if (wrote == false) {
                                strcat(msg, numStr);
                                strcat(msg, " steps");
                                stringColor(screen, DIST, DIST+(row++)*DIST, msg, ORANGE);
                                wrote = true;
                            }
                            stringColor(screen, DIST, DIST+(row++)*DIST, player.name, WHITE);

                        }
                    }
                    if (!wrote) {
                        strcat(msg, "??? steps");
                        stringColor(screen, DIST, DIST+(row++)*DIST, msg, ORANGE);
                        stringColor(screen, DIST, DIST+(row++)*DIST, "No one has completed in minimal number of steps.", WHITE);
                    }
                    fclose(fp2);
                }
            }
            fclose(fp);

        }
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN:   switch(event->key.keysym.sym) {
                                case SDLK_ESCAPE: *gamestate = ResultMenu; init = true; break;
                                default: break;
                            } break;
        case SDL_MOUSEBUTTONUP: switch(event->button.button) {
                                    case SDL_BUTTON_WHEELUP: if (offset < 0) offset += 1; changed = true; break;
                                    case SDL_BUTTON_WHEELDOWN: offset -= 1; changed = true; break;
                                    default: break;
                                } break;
    }
}

/**
 * SÚGÓ
 * screen - képernyõ címe
 * event - esemény
 * gamestate - játékállapot
 */
void help(SDL_Surface *screen, SDL_Event *event, GameState *gamestate) {
    static bool init = true, changed;
    static int offset;
    int rows = 1;

    ///INIT
    if (init) {
        offset = 0;
        changed = true;
        init = false;
    }

    ///OUTPUT
    if (changed) {
        boxColor(screen, 0, 0, WIDTH, HEIGHT, BLACK);
        stringColor(screen, DIST, (offset+rows++)*DIST, "*** WELCOME TO SOKOBAN! ***", ORANGE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Read this little help first, if you are new here.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Game Rules", GREEN);
        stringColor(screen, DIST, (offset+rows++)*DIST, "You have to move boxes with your character.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "A level is completed if each target-field contains a box.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "You are allowed to move only one box at a time.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Boxes might get stuck. You can reset a level at any time.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Game Controls", GREEN);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Arrow keys - move character", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "R - reset level", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "W A S D - move camera", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "K - place your character into the center of the screen", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "ESC - return to menu", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Level Editor", GREEN);
        stringColor(screen, DIST, (offset+rows++)*DIST, "You can create your own levels and play them in Casual Mode.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Drag the fields from the palette onto the screen.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "You can delete any field by left-clicking on it.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Move the camera by pressing W, A, S, D keys.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Before saving a level, you must validate it.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "Press Enter to validate and complete the level.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "After that, in the Editor, press M to save level.", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "You cannot overwrite built-in levels!", WHITE);
        stringColor(screen, DIST, (offset+rows++)*DIST, "You can press T to remove unnecessary walls.", WHITE);
        SDL_Flip(screen);
        changed = false;
    }

    ///INPUT
    SDL_WaitEvent(event);

    ///PROCESS INPUT
    switch(event->type) {
        case SDL_KEYDOWN:   switch(event->key.keysym.sym) {
                                case SDLK_ESCAPE: *gamestate = MainMenu; changed = true; break;
                                default: break;
                            } break;
        case SDL_MOUSEBUTTONUP: switch(event->button.button) {
                                    case SDL_BUTTON_WHEELDOWN: if (offset < 0) offset += 1; changed = true; break;
                                    case SDL_BUTTON_WHEELUP: offset -= 1; changed = true; break;
                                    default: break;
                                } break;
    }
}
