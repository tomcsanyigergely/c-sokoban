#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

typedef char Name[NAME_LENGTH+1];

typedef char FileName[NAME_LENGTH+20+1];

typedef struct Data {
    Name name;
    int value;
} Data;

typedef struct DataList {
    Name name;
    struct DataList* next;
} DataList;

bool completed(char *, char *);
bool nameUsed(char* , char*);
int minStepOfLevel(char*);
void saveResult(char*, char*, int, int);
void savePlayer(char*, bool);
void loadLevel(Field***, Vector*, Vector*, char*);
int fieldCode(Field);
void saveLevel(Field **, Vector, char *, int, bool);
bool builtIn(char*);
OptionMenu levelList(char*, bool, GameMode);
OptionMenu playerList(void);

#endif // FILE_H_INCLUDED
