#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Estructuras y enumeraciones ************************************************

enum Valores {HIGHCARD, PAIR, TWOPAIR, THREEOAK, STRAIGHT, FLUSH, FULLHOUSE, 
              FOUROAK, STRAIGHTFLUSH, ROYALFLUSH};

struct Valor
{
    enum Valores valor;
    int kickers[5];    // 1-13
};

struct Carta
{
    int rank;    // 0-12
    int suit;    // 0-3
};

// Variables globales (no exportadas a Python) ********************************

// Tablas preflop:
static _Bool _rangeHero[13][13];    // [x][y], con [0][0] upper left
static _Bool _rangeVillain[13][13];
static _Bool _rangeTMP[13][13];

// Funciones "privadas" *******************************************************

// utils.c
static int          _rankToCoord(char c);
static struct Carta _intToCard(int carta);
static int          _cardToInt(struct Carta carta);
static struct Carta _strToCard(PyObject *carta);
static int          _comparaValores(struct Valor valH, struct Valor valV);
static struct Valor _valorMano(struct Carta *cartas, int n);

// simulations.c
static PyObject * _simHandVsHand(struct Carta cartas[4]);

// rangeparse.c
static int   _rangeParseNextDelim(const char *buffer, int index);
static _Bool _rangeParse(const char *rangeStr, _Bool val);
static _Bool _rangeParsePintaLine(int x0, int y0, int x1, int y1, _Bool val);
static _Bool _rangeParsePintaUp(int x, int y, _Bool val);
static _Bool _rangeParsePintaDown(int x, int y, _Bool val);

// ranges.c
static void       _rangeSetPercent(_Bool r[13][13], int n, int val);
static double     _rangeGetPercent(_Bool r[13][13]);
static void       _rangeClear(_Bool r[13][13]);
static void       _rangeInvert(_Bool r[13][13]);
static void       _rangeCopy(const _Bool source[13][13], _Bool dest[13][13]);
static void       _rangePrintTable(_Bool r[13][13]);
static PyObject * _rangeGetTable(_Bool r[13][13]);
static int        _rangeWriteString(int x0, int y0, int x1, int y1,
                                    char *buffer, int index);
static PyObject * _rangeGetString(_Bool r[13][13]);
static _Bool      _rangeParToCoords(const char *par, int *xptr, int *yptr,
                                    int *xptr2, int *yptr2);
static _Bool      _rangeCoordsToPar(int x, int y, char *par);

// Funciones a exportar *******************************************************

static PyObject *pl_simHandVsHand(PyObject *self, PyObject *args);
static PyObject *pl_rangeSet(PyObject *self, PyObject *args, PyObject *kwargs);
static PyObject *pl_rangeGetPercent(PyObject *self, PyObject *args);
static PyObject *pl_rangeGet(PyObject *self, PyObject *args, PyObject *kwargs);
static PyObject *pl_rangePrintTable(PyObject *self, PyObject *args);
static PyObject *pl_valorMano(PyObject *self, PyObject *args);
static PyObject *test(PyObject *self, PyObject *args, PyObject *kwargs);

