#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Estructuras y enumeraciones ************************************************

enum Valores {HIGHCARD, PAIR, TWOPAIR, THREEOAK, STRAIGHT, FLUSH, FULLHOUSE, FOUROAK,
              STRAIGHTFLUSH, ROYALFLUSH};

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

// Funciones ******************************************************************

struct Carta _pl_intToCard(int carta);
struct Carta _pl_strToCard(PyObject *carta);
struct Valor _pl_valorMano(struct Carta *cartas, int n);

