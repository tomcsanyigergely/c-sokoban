#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include "globals.h"
#include "game.h"

#include "editor.h"

///P�LYASZERKESZT� F�GGV�NYEINEK MODULJA

/**
 * Megsz�molja a szerkeszt�ben az aktu�lis dobozok �s c�lmez�k sz�m�t.
 * boxCount - ide ker�l a dobozok sz�ma
 * targetCount - ide pedig a c�lmez�k sz�ma
 * level - p�lya adatai
 * levelSize - p�lya m�rete
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
 * Automatikusan fallal ker�ti k�rbe a behelyezett mez�t.
 * level - p�lya c�me
 * selectedPos - a behelyezett mez� koordin�t�i
 */
void autoWall(Field **level, Vector selectedPos) {
    int i, j;
    for(i = selectedPos.y-1; i <= selectedPos.y+1; i++)
    for(j = selectedPos.x-1; j <= selectedPos.x+1; j++)
    if (level[i][j].type == Empty) level[i][j].type = Wall;
}

/**
 * Eld�nti egy falr�l, hogy elt�vol�that�-e (azaz j�rhat� mez�vel nem hat�ros).
 * level - p�lya adatai
 * selectedPos - k�rd�ses fal-mez� koordin�t�i
 * levelSize - p�lya m�rete
 * RETURN - logikai igaz, ha a fal-mez� elt�vol�that�
 */
bool removable(Field **level, Vector selectedPos, Vector levelSize) {
    int i, j;
    for(i = max(selectedPos.y-1, 0); i <= min(selectedPos.y+1, levelSize.y-1); i++)
    for(j = max(selectedPos.x-1, 0); j <= min(selectedPos.x+1, levelSize.x-1); j++)
        if (level[i][j].type == Road || level[i][j].type == Target) return false;
    return true;
}

/**
 * Meghat�rozza annak a mez�nek a poz�ci�j�t, amelyikre klikkelt�nk.
 * level - p�lya adatai
 * cursor - a kurzor poz�ci�ja
 * offset - eltol�s m�rt�ke
 * levelSize - p�lya m�rete
 * selected - a kiv�lasztott mez� c�me ide ker�l
 * RETURN - logikai igaz, ha a p�ly�n bel�l kattintottunk
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
 * Lek�rdezi a kurzor poz�ci�j�t.
 * cursor - ide ker�lnek a kurzor koordin�t�i
 * event - az esem�ny adatai
 */
void setCursor(Vector *cursor, SDL_Event *event){
    *cursor = (Vector){event->button.x, event->button.y};
}

/**
 * Leellen�rzi, hogy a p�lya tartalmaz-e karaktert.
 * level - p�lya adatai
 * levelSize - p�lya m�rete
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
 * Kit�rli egy mez� tartalm�t.
 * field - a mez� c�me
 * hasPlayer - �rt�ke logikai hamiss� v�lik, ha a mez� j�t�kost tartalmazott, k�l�nben v�ltozatlan
 * boxCount - �rt�ke eggyel cs�kken, ha a mez� dobozt tartalmazott, k�l�nben v�ltozatlan
 */
void clearContent(Field *field, bool *hasPlayer, int *boxCount){
    switch(field->content){
        case Player: *hasPlayer = false; break;
        case Box: (*boxCount)--; break;
    }
    field->content = None;
}

/**
 * Egy mez� t�pus�t �resre �ll�tja.
 * field - a mez� c�me
 * targetCount - �rt�ke eggyel cs�kken, ha a kit�r�lt mez� eredetileg c�l-mez� volt
 */
void clearType(Field *field, int *targetCount) {
    if (field->type == Target) (*targetCount)--;
    field->type = Empty;
}

/**
 * Kit�rli a felesleges falakat.
 * level - a p�lya mem�riac�me
 * levelSize - a p�lya m�rete
 */
void clearWall(Field **level, Vector levelSize) {
    int i;
    for(i = 0; i < levelSize.y; i++) {
        int j;
        for(j = 0; j < levelSize.x; j++)
            if (level[i][j].type == Wall && removable(level, (Vector){j, i}, levelSize)) clearType(&level[i][j], NULL);

    }
}
