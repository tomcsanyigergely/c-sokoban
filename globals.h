#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

/// KONSTANS �RT�KEK

enum {
    /// ablak m�rete
    WIDTH = 640,
    HEIGHT = 480,

    /// mez� m�rete
    FIELD_SIZE = 32,

    /// nevek maxim�lis hossza (j�t�kos- �s p�lyan�v)
    NAME_LENGTH = 30,

    /// bemeneti ablak m�rete
    DIALOG_WIDTH = 400,
    DIALOG_HEIGHT = 120,

    /// bemeneti mez� m�rete
    INPUT_WIDTH = 350,
    INPUT_HEIGHT = 30,

    /// t�vols�g a men� opci�i k�z�tt
    DIST = 20,

    /// a programban haszn�lt sz�nek
    BLACK = 0x000000FF,
    WHITE = 0xFFFFFFFF,
    GREEN = 0x00FF00FF,
    DARKGREEN = 0x002000FF,
    YELLOW = 0xFFFF00FF,
    ORANGE = 0xF0B000FF,
    GREY = 0x303030FF
};

/// mez�k�dok �rt�ke
#define EMPTY 0 /// �res mez�
#define WALL 1 /// fal
#define ROAD 2 /// �res �t
#define BOX 3 /// dobozt tartalmaz� �t
#define TARGET 4 /// �res c�l-mez�
#define PLAYER 5 /// j�t�kost tartalmaz� �t
#define BOX_ON_TARGET 6 /// dobozt tartalmaz� c�l-mez�
#define PLAYER_ON_TARGET 7 /// j�t�kost tartalmaz� c�l-mez�

#endif // GLOBALS_H_INCLUDED
