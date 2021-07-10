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

struct Coords
{
    int x, y;    // tabla preflop, con 0,0 en upper left
};

// Variables globales (no exportadas a Python) ********************************

// Tabla preflop:
static _Bool _range[13][13];    // [x][y], con [0][0] upper left

// Funciones "privadas" *******************************************************

// utils.c
static int          _rankToNum(char c);
static struct Carta _intToCard(int carta);
static struct Carta _strToCard(PyObject *carta);
static struct Valor _valorMano(struct Carta *cartas, int n);

// ranges.c
static void          _rangeSetPercent(int n, int val);
static void          _rangeClear();
static void          _rangeInvert();
static void          _rangePrintTable();
static void          _rangePrintSimpleTable();
static void          _rangePrintRange();
static int           _rangeRankToCoord(char c);
static struct Coords _rangeParToCoords(const char *par);
static void          _rangeCoordsToPar(struct Coords c, char *par, _Bool onlyInRange);

// Funciones a exportar *******************************************************

static PyObject *pl_rangePercent(PyObject *self, PyObject *args);
static PyObject *pl_rangePrint(PyObject *self, PyObject *args, PyObject *kwargs);
static PyObject *pl_valorMano(PyObject *self, PyObject *args);
static PyObject *test(PyObject *self, PyObject *args, PyObject *kwargs);

