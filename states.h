#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

typedef char Option[20+1];
typedef char NumberStr[6];
typedef char Message[40+1];

typedef struct OptionMenu {
    int optionNumber;
    int choice;
    Option* options;
} OptionMenu;

typedef enum GameState {
    MainMenu,
    SelectPlayer,
    SelectLevelType,
    SelectLevel,
    PlayLevel,

    EditorMenu,
    CreateLevel,
    EditLevel,
    AskInput,

    ResultMenu,
    ResultList,
    Help,
    Close
} GameState;

typedef enum GameMode {
    Editor,
    Play
} GameMode;

typedef enum ResultMode {
    Players,
    Levels
} ResultMode;

OptionMenu newOptionMenu(int, Option*);
void mainMenu(SDL_Surface*, SDL_Event*, GameState*, GameMode*);
void selectPlayer(SDL_Surface*, SDL_Event*, char*, GameState*);
void selectLevelType(SDL_Surface*, SDL_Event*, bool*, GameState*);
void selectLevel(SDL_Surface*, SDL_Event*, Field***, Vector*, Vector*, char*, char*, int*, bool, GameState*, GameMode);
void playLevel(SDL_Surface*, SDL_Event*, Field**, Vector, Vector, GameState*, GameMode, bool*, int*, char*, char*);
void askInput(SDL_Surface*, SDL_Event*, Field**, Vector, int, GameState*, GameMode);
void editorMenu(SDL_Surface*, SDL_Event*, GameState*, bool*);
void createLevelMenu(SDL_Surface*, SDL_Event*, GameState*, Field***, Vector*, int*);
void editLevel(SDL_Surface*, SDL_Event*, Field**, Vector, Vector*, GameState*, bool*, int*);
void resultMenu(SDL_Surface*, SDL_Event*, ResultMode*, GameState*);
void resultList(SDL_Surface*, SDL_Event*, GameState*, ResultMode);
void help(SDL_Surface*, SDL_Event*, GameState*);

#endif // MENU_H_INCLUDED
