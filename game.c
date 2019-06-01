#include <stdbool.h>

#include "game.h"

/// J�T�K K�ZBEN GYAKRAN HASZN�LT F�GGV�NYEK

/**
 * Visszaadja k�t sz�m minimum�t
 * a - egyik sz�m
 * b - m�sik sz�m
 * RETURN - a kisebbik sz�ma �rt�ke
 */
int min(int a, int b) {
    return (a < b) ? a : b;
}

/// Az el�z� f�ggv�nyhez hasonl� - k�t sz�mn maximum�t adja vissza.
int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * A karakter l�ptet�s�t, �s a dobozok eltol�s�t v�gzi el.
 * level - a p�lya c�me
 * pos - a karakter poz�ci�j�nak c�me
 * offset - az eltol�s m�rt�k�nek c�me
 * dir - a karakter ir�nya
 * RETURN - logikai igaz, ha a karakter tudott mozogni a megadott ir�nyba
 */
bool move(Field **level, Vector *pos, Vector *offset, Vector dir) {
    bool moved = false;
    int x = pos->x;
    int y = pos->y;
    Field* actual = &level[y][x];
    Field* next = &level[y+dir.y][x+dir.x];
    if (next->type != Wall) {
        if (next->content == None) {
            actual->content = None;
            next->content = Player;
            pos->x += dir.x;
            pos->y += dir.y;
            moved = true;
        }
        else if (next->content == Box){
            Field* nextafter = &level[y+2*dir.y][x+2*dir.x];
            if (nextafter->type != Wall && nextafter->content != Box) {
                actual->content = None;
                next->content = Player;
                nextafter->content = Box;
                pos->x += dir.x;
                pos->y += dir.y;
                moved = true;
            }
        }
    }
    return moved;
}

/**
 * Mem�riahelyet foglal egy megadott m�ret� p�lya r�sz�re.
 * level - p�lya mem�riac�me
 * levelSize - p�lya m�rete
 */
void mallocLevel(Field ***level, Vector levelSize) {
    *level = (Field**) malloc(levelSize.y * sizeof(Field*));
    int i;
    for(i = 0; i < levelSize.y; i++)
        (*level)[i] = (Field*) malloc(levelSize.x * sizeof(Field));
}

/**
 * Felszabad�tja a p�lya r�sz�re foglalt mem�ri�t.
 * level - a p�lya mem�riac�me
 * levelSize - a p�lya m�rete
 */
void freeLevel(Field **level, Vector levelSize) {
    int i;
    for(i = 0; i < levelSize.y; i++) free(level[i]);
    free(level);
}

/**
 * �jraind�t egy p�ly�t.
 * fromLevel - a p�lya kezd��llapota
 * toLevel - a be�ltend� p�lya mem�riac�me
 * levelSize - a p�lya m�rete
 */
void resetLevel(Field **fromLevel, Field ***toLevel, Vector levelSize) {
    mallocLevel(toLevel, levelSize);
    int i, j;
    for(i = 0; i < levelSize.y; i++)
    for(j = 0; j < levelSize.x; j++) {
        (*toLevel)[i][j].type = fromLevel[i][j].type;
        (*toLevel)[i][j].content = fromLevel[i][j].content;
    }
}

/**
 * Ellen�rzi, hogy a p�lya teljes�tve van-e.
 * level - p�lya adatai
 * levleSize - p�lya m�rete
 * RETURN - logikai igaz, ha a p�lya teljes�tve van
 */
bool test(Field **level, Vector levelSize){
    int i;
    bool valid = true;
    for(i = 0; i < levelSize.y && valid; i++) {
        int j;
        for(j = 0; j < levelSize.x && !(level[i][j].type == Target && level[i][j].content != Box); j++);
        valid = (j == levelSize.x);
    }
    return valid;
}
