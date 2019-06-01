#include <stdbool.h>

#include "game.h"

/// JÁTÉK KÖZBEN GYAKRAN HASZNÁLT FÜGGVÉNYEK

/**
 * Visszaadja két szám minimumát
 * a - egyik szám
 * b - másik szám
 * RETURN - a kisebbik száma értéke
 */
int min(int a, int b) {
    return (a < b) ? a : b;
}

/// Az elõzõ függvényhez hasonló - két számn maximumát adja vissza.
int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * A karakter léptetését, és a dobozok eltolását végzi el.
 * level - a pálya címe
 * pos - a karakter pozíciójának címe
 * offset - az eltolás mértékének címe
 * dir - a karakter iránya
 * RETURN - logikai igaz, ha a karakter tudott mozogni a megadott irányba
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
 * Memóriahelyet foglal egy megadott méretû pálya részére.
 * level - pálya memóriacíme
 * levelSize - pálya mérete
 */
void mallocLevel(Field ***level, Vector levelSize) {
    *level = (Field**) malloc(levelSize.y * sizeof(Field*));
    int i;
    for(i = 0; i < levelSize.y; i++)
        (*level)[i] = (Field*) malloc(levelSize.x * sizeof(Field));
}

/**
 * Felszabadítja a pálya részére foglalt memóriát.
 * level - a pálya memóriacíme
 * levelSize - a pálya mérete
 */
void freeLevel(Field **level, Vector levelSize) {
    int i;
    for(i = 0; i < levelSize.y; i++) free(level[i]);
    free(level);
}

/**
 * Újraindít egy pályát.
 * fromLevel - a pálya kezdõállapota
 * toLevel - a beöltendõ pálya memóriacíme
 * levelSize - a pálya mérete
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
 * Ellenõrzi, hogy a pálya teljesítve van-e.
 * level - pálya adatai
 * levleSize - pálya mérete
 * RETURN - logikai igaz, ha a pálya teljesítve van
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
