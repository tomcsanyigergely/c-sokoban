#ifndef GLOBALS_H_INCLUDED
#define GLOBALS_H_INCLUDED

/// KONSTANS ÉRTÉKEK

enum {
    /// ablak mérete
    WIDTH = 640,
    HEIGHT = 480,

    /// mezõ mérete
    FIELD_SIZE = 32,

    /// nevek maximális hossza (játékos- és pályanév)
    NAME_LENGTH = 30,

    /// bemeneti ablak mérete
    DIALOG_WIDTH = 400,
    DIALOG_HEIGHT = 120,

    /// bemeneti mezõ mérete
    INPUT_WIDTH = 350,
    INPUT_HEIGHT = 30,

    /// távolság a menü opciói között
    DIST = 20,

    /// a programban használt színek
    BLACK = 0x000000FF,
    WHITE = 0xFFFFFFFF,
    GREEN = 0x00FF00FF,
    DARKGREEN = 0x002000FF,
    YELLOW = 0xFFFF00FF,
    ORANGE = 0xF0B000FF,
    GREY = 0x303030FF
};

/// mezõkódok értéke
#define EMPTY 0 /// üres mezõ
#define WALL 1 /// fal
#define ROAD 2 /// üres út
#define BOX 3 /// dobozt tartalmazó út
#define TARGET 4 /// üres cél-mezõ
#define PLAYER 5 /// játékost tartalmazó út
#define BOX_ON_TARGET 6 /// dobozt tartalmazó cél-mezõ
#define PLAYER_ON_TARGET 7 /// játékost tartalmazó cél-mezõ

#endif // GLOBALS_H_INCLUDED
