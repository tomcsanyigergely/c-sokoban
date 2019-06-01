#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL.h>
#include "globals.h"
#include "game.h"
#include "states.h"
#include "file.h"

///FÁJLKEZELÕ MODUL

/**
 * Meghatározza egy megadott pályáról, hogy azt teljesítette-e a megadott játékos.
 * playerName - játékos neve
 * levelName - pálya neve
 * RETURN - logikai igaz, ha a játékos teljesítette a megadott pályát
 */
bool completed(char *playerName, char *levelName) {
    FileName file;
    strcpy(file, "data/results/");
    strcat(file, levelName);
    strcat(file, ".txt");

    FILE* fp = fopen(file, "r");
    int dataNumber;
    fscanf(fp, " %d", &dataNumber);
    Data player;
    int i;
    bool found = false;
    for(i = 0; i < dataNumber && !found; i++) {
        fscanf(fp, " %s %d", player.name, &player.value);
        if (strcmp(player.name, playerName) == 0) found = true;
    }
    fclose(fp);
    return found;
}

/**
 * Eldönti egy névrõl, hogy a megadott fájl tartalmazza-e.
 * name - játékos, vagy pálya neve
 * file - a fájl elérési útja
 * RETURN - logikai igaz, ha a fájl tartalmazza a nevet.
 */
bool nameUsed(char* name, char* file) {
    FILE* fp = fopen(file, "r");
    int number;
    fscanf(fp, " %d", &number);
    Data data;
    data.name[0] = '\0';
    bool found = false;
    int i;
    for(i = 0; i < number && !found; i++)
    {
        fscanf(fp, " %s %d", data.name, &data.value);
        if (strcmp(name, data.name) == 0) found = true;
    }
    fclose(fp);
    return found;
}

/**
 * Meghatározza egy pálya eddigi legkevesebb lépésszámát.
 * levelName - pálya neve
 * RETURN - a legkevesebb lépésszám
 */
int minStepOfLevel(char *levelName) {
    FILE* fp = fopen("data/levels.txt", "r");
    int levelNumber;
    fscanf(fp, " %d", &levelNumber);
    bool found = false;
    Data levelData;
    int i;
    for(i = 0; !found; i++) {
        fscanf(fp, " %s %d", levelData.name, &levelData.value);
        if (strcmp(levelData.name, levelName) == 0) found = true;
    }
    fclose(fp);
    return levelData.value;
}

/**
 * Elmenti egy játékos eredményét, és felülírja a pályához tartozó eddigi legkevesebb lépésszámot, ha szükséges.
 * levelName - pálya neve
 * playerName - játékos neve
 * step - játékos lépésszáma
 * minStep - eddigi legkevesebb lépésszám
 */
void saveResult(char* levelName, char* playerName, int step, int minStep) {
    Data* players = NULL;
    int playerIndex = -1; /// -1 = playerName not found

    FileName file;
    strcpy(file, "data/results/");
    strcat(file, levelName);
    strcat(file, ".txt");

    ///READ FROM results/<level>.txt
    FILE* fp = fopen(file, "r");
    int playerNumber;
    fscanf(fp, " %d", &playerNumber);
    if (playerNumber > 0) {
        players = (Data*)malloc(playerNumber*sizeof(Data));
        int i;
        for(i = 0; i < playerNumber; i++) {
            fscanf(fp, " %s %d", players[i].name, &players[i].value);
            if (strcmp(playerName, players[i].name) == 0) playerIndex = i;
        }
    }
    fclose(fp);

    if (playerIndex != -1) players[playerIndex].value = min(players[playerIndex].value, step);

    ///WRITE INTO results/<level>.txt
    fp = fopen(file, "w");
    fprintf(fp, "%d\n", playerIndex == -1 ? playerNumber+1 : playerNumber);
    int i;
    for(i = 0; i < playerNumber; i++) fprintf(fp, "%s %d\n", players[i].name, players[i].value);
    if (playerIndex == -1) fprintf(fp, "%s %d", playerName, step);
    fclose(fp);
    free(players);

    if (playerIndex == -1) {
        ///READ FROM players.txt
        fp = fopen("data/players.txt", "r");
        fscanf(fp, " %d", &playerNumber);
        players = (Data*)malloc(playerNumber*sizeof(Data));
        int i;
        for(i = 0; i < playerNumber; i++) {
            fscanf(fp, " %s %d", players[i].name, &players[i].value);
            if (strcmp(players[i].name, playerName) == 0) playerIndex = i;
        }
        fclose(fp);
        players[playerIndex].value++;

        ///WRITE INTO players.txt
        fp = fopen("data/players.txt", "w");
        fprintf(fp, "%d\n", playerNumber);
        for(i = 0; i < playerNumber; i++) fprintf(fp, "%s %d\n", players[i].name, players[i].value);
        fclose(fp);
        free(players);
    }

    ///UPDATE MINIMAL STEP IN levels.txt
    if (step < minStep) {
        fp = fopen("data/levels.txt", "r");
        int levelNumber;
        fscanf(fp, " %d", &levelNumber);
        Data* levels = (Data*)malloc(levelNumber*sizeof(Data));
        int i;
        for(i = 0; i < levelNumber; i++) {
            fscanf(fp, " %s %d", levels[i].name, &levels[i].value);
            if (strcmp(levelName, levels[i].name) == 0) levels[i].value = step;
        }
        fclose(fp);

        fp = fopen("data/levels.txt", "w");
        fprintf(fp, "%d\n", levelNumber);
        for(i = 0; i < levelNumber; i++) fprintf(fp, "%s %d\n", levels[i].name, levels[i].value);
        fclose(fp);
        free(levels);
    }
}

/**
 * Elmenti egy játékos nevét.
 * name - játékos neve
 * overwrite - logikai igaz, ha a játékos neve már használt
 */
void savePlayer(char* name, bool overwrite) {
    int playerNumber, i;

    ///READ
    FILE* fp = fopen("data/players.txt", "r");
    fscanf(fp, " %d", &playerNumber);
    Data* players = NULL;
    if (playerNumber > 0) {
        players = (Data*)malloc(playerNumber*sizeof(Data));
        for(i = 0; i < playerNumber; i++) {
            fscanf(fp, " %s %d", players[i].name, &players[i].value);
        }
    }
    fclose(fp);

    ///WRITE
    fp = fopen("data/players.txt", "w");
    fprintf(fp, "%d\n", overwrite ? playerNumber : playerNumber+1);
    for (i = 0; i < playerNumber; i++) fprintf(fp, "%s %d\n", players[i].name, strcmp(name, players[i].name) == 0 ? 0 : players[i].value);
    if (!overwrite) fprintf(fp, "%s %d\n", name, 0);
    fclose(fp);
    free(players);
}

/**
 * Betölti egy megadott nevû pálya adatait.
 * level - ide kerülnek a pálya adatai
 * levelSize - ide kerül a pálya mérete
 * startPos - ide kerül a karakter kiindulási pozíciója
 * levelName - betöltendõ pálya neve
 */
void loadLevel(Field ***level, Vector *levelSize, Vector *startPos, char *levelName) {
    ///FIND LEVEL DATA
    FileName path;
    strcpy(path, "data/levels/");
    strcat(path, levelName);
    strcat(path, ".txt");
    FILE* fp = fopen(path, "r");
    int rows, columns;

    fscanf(fp, " %d %d", &rows, &columns);

    levelSize->x = columns;
    levelSize->y = rows;

    mallocLevel(level, *levelSize);

    int inp;
    int i, j;
    Field* actual;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < columns; j++) {
            actual = &(*level)[i][j];
            fscanf(fp," %d",&inp);
            switch(inp){
                case EMPTY:
                    actual->type = Empty;
                    actual->content = None;
                    break;
                case WALL:
                    actual->type = Wall;
                    actual->content = None;
                    break;
                case ROAD:
                    actual->type = Road;
                    actual->content = None;
                    break;
                case BOX:
                    actual->type = Road;
                    actual->content = Box;
                    break;
                case PLAYER:
                    actual->type = Road;
                    actual->content = Player;
                    startPos->x = j;
                    startPos->y = i;
                    break;
                case TARGET:
                    actual->type = Target;
                    actual->content = None;
                    break;
                case BOX_ON_TARGET:
                    actual->type = Target;
                    actual->content = Box;
                    break;
                case PLAYER_ON_TARGET:
                    actual->type = Target;
                    actual->content = Player;
                    startPos->x = j;
                    startPos->y = i;
                    break;
            }
        }
    }
    fclose(fp);
}

/**
 * Meghatározza egy mezõ kódját (EMPTY = 0, WALL = 1, stb. Lásd: globals.h makródefiníciói).
 * field - a mezõ adatai
 * RETURN - a mezõnek megfelelõ kód értéke
 */
int fieldCode(Field field){
    if (field.content == None) switch (field.type) {
        case Empty: return EMPTY; break;
        case Wall:  return WALL;  break;
        case Road:  return ROAD;  break;
        case Target: return TARGET; break;
    }
    else if (field.content == Box) switch (field.type) {
        case Road: return BOX; break;
        case Target: return BOX_ON_TARGET; break;
    }
    else if (field.content == Player) switch(field.type) {
        case Road: return PLAYER; break;
        case Target: return PLAYER_ON_TARGET; break;
    }
}

/**
 * Elment egy pályát a megadott névvel.
 * level - pálya adatai
 * levelSize - pálya mérete
 * name - pálya neve
 * minStep - legkevesebb lépések száma
 * overwrite - értéke igaz, ha egy pályát felülírunk
 */
void saveLevel(Field **level, Vector levelSize, char *name, int minStep, bool overwrite){
    int levelNumber, i;
    FILE* fp;

    ///READ
    fp = fopen("data/levels.txt", "r");
    fscanf(fp, " %d", &levelNumber);
    Data* levels = NULL;
    if (levelNumber > 0) {
        levels = (Data*)malloc(levelNumber*sizeof(Data));
        for (i = 0; i < levelNumber; i++) {
            fscanf(fp, " %s %d", levels[i].name, &levels[i].value);
        }
    }
    fclose(fp);

    ///WRITE into levels.txt
    fp = fopen("data/levels.txt", "w");
    fprintf(fp, "%d\n", overwrite ? levelNumber : levelNumber+1);
    for (i = 0; i < levelNumber; i++) fprintf(fp, "%s %d\n", levels[i].name, strcmp(name, levels[i].name) == 0 ? minStep : levels[i].value);
    if (!overwrite) fprintf(fp, "%s %d\n", name, minStep);
    fclose(fp);

    free(levels);

    ///WRITE into <levelName>.txt
    FileName path;
    strcpy(path, "data/levels/");
    strcat(path, name);
    strcat(path, ".txt");

    fp = fopen(path, "w");
    fprintf(fp, "%d %d\n", levelSize.y, levelSize.x);
    for(i = 0; i < levelSize.y; i++) {
        fprintf(fp, "%d", fieldCode(level[i][0]));
        int j;
        for(j = 1; j < levelSize.x; j++) {
            fprintf(fp, " %d", fieldCode(level[i][j]));
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    strcpy(path, "data/results/");
    strcat(path, name);
    strcat(path, ".txt");
    fp = fopen(path, "w");
    fprintf(fp, "0\n");
    fclose(fp);
}

/**
 * Eldönti egy pályáról, hogy beépített pálya-e.
 * levelName - pálya neve
 * RETURN - logikai igaz, ha a pálya beépített
 */
bool builtIn(char* levelName) {
    FILE* fp = fopen("data/level_list.txt", "r");
    int rows;
    fscanf(fp, " %d", &rows);
    bool found = false;
    int i;
    for(i = 0; i < rows && !found; i++) {
        Name level;
        fscanf(fp, " %s", level);
        if (strcmp(level, levelName) == 0) found = true;
    }
    fclose(fp);
    return found;
}

/**
 * Létrehoz egy listamenüt a pályanevekkel.
 * playerName - játékos neve
 * builtInLevels - értéke logikai igaz, ha a játékos beépített pályákkal akar játszani
 * gamemode - a játékmódot tartalmazza (Play = játék vagy Editor = szerkesztõ)
 * RETURN - a létrehozott listamenü-struktúra
 */
OptionMenu levelList(char* playerName, bool builtInLevels, GameMode gamemode) {
    OptionMenu menu;
    menu.choice = 0;
    DataList* list = NULL;
    DataList** listptr = &list;

    if (builtInLevels && gamemode == Play) {
        FILE* fp = fopen("data/level_list.txt", "r");
        int rows;
        fscanf(fp, " %d", &rows);
        bool read = true;
        int i = 0;
        while(read && i < rows) {
            (*listptr) = (DataList*)malloc(sizeof(DataList));
            fscanf(fp, " %s", (*listptr)->name);
            (*listptr)->next = NULL;
            if (!completed(playerName, (*listptr)->name)) read = false;
            listptr = &(*listptr)->next;
            i++;
        }
        menu.optionNumber = i;
        fclose(fp);
    }
    else {
        menu.optionNumber = 0;
        FILE* fp = fopen("data/levels.txt", "r");
        int rows;
        fscanf(fp, " %d", &rows);
        bool read = true;
        int i;
        for(i = 0; i < rows; i++) {
            Name levelName;
            fscanf(fp, " %s %*d", levelName);
            if (!builtIn(levelName) || gamemode == Editor) {
                (*listptr) = (DataList*)malloc(sizeof(DataList));
                strcpy((*listptr)->name, levelName);
                (*listptr)->next = NULL;
                listptr = &(*listptr)->next;
                menu.optionNumber++;
            }
        }
        fclose(fp);
    }

    ///COPY
    menu.options = (Option*)malloc((menu.optionNumber+1)*sizeof(Option));
    int i;
    DataList* ptr = list;
    for(i = 0; i < menu.optionNumber; i++) {
        strcpy(menu.options[i], ptr->name);
        DataList* next = ptr->next;
        free(ptr);
        ptr = next;
    }
    strcpy(menu.options[menu.optionNumber], "Cancel");
    menu.optionNumber++;
    ///RETURN
    return menu;
}

/**
 * Létrehoz egy listamenüt a játékosnevekkel.
 * RETURN - a létrehozott listamenü-struktúra
 */
OptionMenu playerList(void){
    OptionMenu menu;
    menu.choice = 0;
    FILE* fp = fopen("data/players.txt", "r");
    fscanf(fp, " %d", &menu.optionNumber);
    menu.optionNumber += 2;
    menu.options = (Option*)malloc((menu.optionNumber)*sizeof(Option));
    strcpy(menu.options[0], "New Profile");
    strcpy(menu.options[menu.optionNumber-1], "Cancel");
    int i;
    for(i = 1; i < menu.optionNumber-1; i++) fscanf(fp, " %s %*d", menu.options[i]);
    fclose(fp);
    return menu;
}
