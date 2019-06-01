#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "globals.h"
#include "game.h"

#include "editor.h"

///PÁLYASZERKESZTÕ FÜGGVÉNYEINEK MODULJA

/**
 * Megszámolja a szerkesztõben az aktuális dobozok és célmezõk számát.
 * boxCount - ide kerül a dobozok száma
 * targetCount - ide pedig a célmezõk száma
 * level - pálya adatai
 * levelSize - pálya mérete
 */
void count(int *boxCount, int *targetCount, Field **level, Vector levelSize) {
    *boxCount = 0;
    *targetCount = 0;
    int i, j;
    for(i = 0; i < levelSize.y; i++)
    for(j = 0; j < levelSize.x; j++){
        if(level[i][j].content == Box) (*boxCount)++;
        if(level[i][j].type == Target) (*targetCount)++;
    }
}

/**
 * Automatikusan fallal keríti körbe a behelyezett mezõt.
 * level - pálya címe
 * selectedPos - a behelyezett mezõ koordinátái
 */
void autoWall(Field **level, Vector selectedPos) {
    int i, j;
    for(i = selectedPos.y-1; i <= selectedPos.y+1; i++)
    for(j = selectedPos.x-1; j <= selectedPos.x+1; j++)
    if (level[i][j].type == Empty) level[i][j].type = Wall;
}

/**
 * Eldönti egy falról, hogy eltávolítható-e (azaz járható mezõvel nem határos).
 * level - pálya adatai
 * selectedPos - kérdéses fal-mezõ koordinátái
 * levelSize - pálya mérete
 * RETURN - logikai igaz, ha a fal-mezõ eltávolítható
 */
bool removable(Field **level, Vector selectedPos, Vector levelSize) {
    int i, j;
    for(i = max(selectedPos.y-1, 0); i <= min(selectedPos.y+1, levelSize.y-1); i++)
    for(j = max(selectedPos.x-1, 0); j <= min(selectedPos.x+1, levelSize.x-1); j++)
        if (level[i][j].type == Road || level[i][j].type == Target) return false;
    return true;
}

/**
 * Meghatározza annak a mezõnek a pozícióját, amelyikre klikkeltünk.
 * level - pálya adatai
 * cursor - a kurzor pozíciója
 * offset - eltolás mértéke
 * levelSize - pálya mérete
 * selected - a kiválasztott mezõ címe ide kerül
 * RETURN - logikai igaz, ha a pályán belül kattintottunk
 */
bool findSelectedField(Field **level, Vector cursor, Vector offset, Vector levelSize, Vector *selectedPos, Field **selected) {
    int x = (cursor.x-offset.x)/FIELD_SIZE;
    int y = (cursor.y-offset.y)/FIELD_SIZE;
    if (x >= 0 && x < levelSize.x && y >= 0 && y < levelSize.y){
        selectedPos->x = x;
        selectedPos->y = y;
        *selected = &level[y][x];
        return true;
    }
    else return false;
}

/**
 * Lekérdezi a kurzor pozícióját.
 * cursor - ide kerülnek a kurzor koordinátái
 * event - az esemény adatai
 */
void setCursor(Vector *cursor, SDL_Event *event){
    *cursor = (Vector){event->button.x, event->button.y};
}

/**
 * Leellenõrzi, hogy a pálya tartalmaz-e karaktert.
 * level - pálya adatai
 * levelSize - pálya mérete
 * RETURN - logikai igaz, ha tartalmaz karaktert
 */
bool testHasPlayer(Field **level, Vector levelSize) {
    int i;
    bool hasPlayer = false;
    for(i = 0; i < levelSize.y && !hasPlayer; i++) {
        int j;
        for(j = 0; j < levelSize.x && !(level[i][j].content == Player); j++);
        hasPlayer = !(j == levelSize.x);
    }
    return hasPlayer;
}

/**
 * Kitörli egy mezõ tartalmát.
 * field - a mezõ címe
 * hasPlayer - értéke logikai hamissá válik, ha a mezõ játékost tartalmazott, különben változatlan
 * boxCount - értéke eggyel csökken, ha a mezõ dobozt tartalmazott, különben változatlan
 */
void clearContent(Field *field, bool *hasPlayer, int *boxCount){
    switch(field->content){
        case Player: *hasPlayer = false; break;
        case Box: (*boxCount)--; break;
    }
    field->content = None;
}

/**
 * Egy mezõ típusát üresre állítja.
 * field - a mezõ címe
 * targetCount - értéke eggyel csökken, ha a kitörölt mezõ eredetileg cél-mezõ volt
 */
void clearType(Field *field, int *targetCount) {
    if (field->type == Target) (*targetCount)--;
    field->type = Empty;
}

/**
 * Kitörli a felesleges falakat.
 * level - a pálya memóriacíme
 * levelSize - a pálya mérete
 */
void clearWall(Field **level, Vector levelSize) {
    int i;
    for(i = 0; i < levelSize.y; i++) {
        int j;
        for(j = 0; j < levelSize.x; j++)
            if (level[i][j].type == Wall && removable(level, (Vector){j, i}, levelSize)) clearType(&level[i][j], NULL);

    }
}
