#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL.h>
#include "globals.h"
#include "game.h"
#include "states.h"
#include "file.h"

///F�JLKEZEL� MODUL

/**
 * Meghat�rozza egy megadott p�ly�r�l, hogy azt teljes�tette-e a megadott j�t�kos.
 * playerName - j�t�kos neve
 * levelName - p�lya neve
 * RETURN - logikai igaz, ha a j�t�kos teljes�tette a megadott p�ly�t
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
 * Eld�nti egy n�vr�l, hogy a megadott f�jl tartalmazza-e.
 * name - j�t�kos, vagy p�lya neve
 * file - a f�jl el�r�si �tja
 * RETURN - logikai igaz, ha a f�jl tartalmazza a nevet.
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
 * Meghat�rozza egy p�lya eddigi legkevesebb l�p�ssz�m�t.
 * levelName - p�lya neve
 * RETURN - a legkevesebb l�p�ssz�m
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
 * Elmenti egy j�t�kos eredm�ny�t, �s fel�l�rja a p�ly�hoz tartoz� eddigi legkevesebb l�p�ssz�mot, ha sz�ks�ges.
 * levelName - p�lya neve
 * playerName - j�t�kos neve
 * step - j�t�kos l�p�ssz�ma
 * minStep - eddigi legkevesebb l�p�ssz�m
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
 * Elmenti egy j�t�kos nev�t.
 * name - j�t�kos neve
 * overwrite - logikai igaz, ha a j�t�kos neve m�r haszn�lt
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
 * Bet�lti egy megadott nev� p�lya adatait.
 * level - ide ker�lnek a p�lya adatai
 * levelSize - ide ker�l a p�lya m�rete
 * startPos - ide ker�l a karakter kiindul�si poz�ci�ja
 * levelName - bet�ltend� p�lya neve
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
 * Meghat�rozza egy mez� k�dj�t (EMPTY = 0, WALL = 1, stb. L�sd: globals.h makr�defin�ci�i).
 * field - a mez� adatai
 * RETURN - a mez�nek megfelel� k�d �rt�ke
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
 * Elment egy p�ly�t a megadott n�vvel.
 * level - p�lya adatai
 * levelSize - p�lya m�rete
 * name - p�lya neve
 * minStep - legkevesebb l�p�sek sz�ma
 * overwrite - �rt�ke igaz, ha egy p�ly�t fel�l�runk
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
 * Eld�nti egy p�ly�r�l, hogy be�p�tett p�lya-e.
 * levelName - p�lya neve
 * RETURN - logikai igaz, ha a p�lya be�p�tett
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
 * L�trehoz egy listamen�t a p�lyanevekkel.
 * playerName - j�t�kos neve
 * builtInLevels - �rt�ke logikai igaz, ha a j�t�kos be�p�tett p�ly�kkal akar j�tszani
 * gamemode - a j�t�km�dot tartalmazza (Play = j�t�k vagy Editor = szerkeszt�)
 * RETURN - a l�trehozott listamen�-strukt�ra
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
 * L�trehoz egy listamen�t a j�t�kosnevekkel.
 * RETURN - a l�trehozott listamen�-strukt�ra
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
