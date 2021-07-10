#include "pokerlab.h"

// No podemos organizar el módulo en object files, porque todos los elementos
// deben tener linkage interno. Por lo tanto solo podemos tener una translation
// unit (no procede Makefile).

// Definición de funciones "privadas" *****************************************

#include "utils.c"
#include "ranges.c"

// Definición de funciones a exportar *****************************************

static PyObject *pl_rangePercent(PyObject *self, PyObject *args)
{
    // Establece tabla preflop a partir de porcentaje
    int n = 0;
    const char *sp = NULL;
    if(!PyArg_ParseTuple(args, "|is", &n, &sp))
    {
        PyErr_SetString(PyExc_TypeError, "Argumentos esperados: int, string.");
        return NULL;
    }
    if(sp && !strcmp(sp, "clear"))
    {
        _rangeClear();
        Py_RETURN_NONE;
    }
    if(sp && !strcmp(sp, "invert"))
    {
        _rangeInvert();
        Py_RETURN_NONE;
    }
    if(n < 0 || n > 100)
    {
        PyErr_SetString(PyExc_TypeError,
            "Se espera un entero 0-100 para acciones set, add y subtract.");
        return NULL;
    }
    if(!sp || !strcmp(sp, "set"))
    {
        _rangeClear();
        _rangeSetPercent(n, 1);
    }
    else if(!strcmp(sp, "add"))
        _rangeSetPercent(n, 1);
    else if(!strcmp(sp, "subtract"))
        _rangeSetPercent(n, 0);
    else
    {
        PyErr_SetString(PyExc_TypeError,
            "Acciones: set (default), add, subtract, clear, invert.");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *pl_rangePrint(PyObject *self, PyObject *args, PyObject *kwargs)
{
    // Muestra range actual en pantalla
    PyObject *tabla = NULL, *tablaSimple = NULL;
    char *keys[] = {"tabla", "tablaSimple", NULL};
    // Borrowed references (tabla, tablaSimple):
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|$OO", keys, &tabla, &tablaSimple))
    {
        PyErr_SetString(PyExc_ValueError,
            "Keyword arguments booleanos (opcionales): tabla, tablaSimple.");
        return NULL;
    }
    if(tabla == Py_True)
        _rangePrintTable();
    else if(tablaSimple == Py_True)
        _rangePrintSimpleTable();
    else
        _rangePrintRange();
    Py_RETURN_NONE;
}
    
static PyObject *pl_valorMano(PyObject *self, PyObject *args)
{
    int i, j;
    struct Carta cartas[7], c;
    struct Valor resultado;
    PyObject *tuplaKickers, *tuplaResul;
    Py_ssize_t numargs = PySequence_Length(args);
    if(numargs != 5 && numargs != 7)
    {
        PyErr_SetString(PyExc_TypeError, "Se esperan 5 o 7 argumentos.");
        return NULL;
    }
    // Vamos a recoger argumentos y  almacenar las cartas en el formato correcto:
    PyObject *arg;
    for(i = 0; i < numargs; i++)
    {
        arg = PyTuple_GetItem(args, i);    // borrowed reference
        if(PyLong_Check(arg))
            c = _intToCard(PyLong_AsLong(arg));
        else if(PyUnicode_Check(arg))
            c = _strToCard(arg);
        else
        {
            PyErr_SetString(PyExc_TypeError, "Argumento debe ser entero o string.");
            return NULL;
        }
        if(c.rank == -1)
        {
            PyErr_SetString(PyExc_ValueError, "Argumento debe ser 0-51 o tipo 'Td'.");
            return NULL;
        }
        cartas[i] = c;
    }
    // Ya tenemos las cartas en 'cartas'
    resultado = _valorMano(cartas, numargs);
    i = 0;
    while(resultado.kickers[i] != -1)
    {
        i++;
        if(i == 5)
            break;
    }
    tuplaKickers = PyTuple_New(i);    // new reference
    for(j = 0; j < i; j++)
        PyTuple_SetItem(tuplaKickers, j, PyLong_FromLong(resultado.kickers[j]));  // steals
    // Retornará nueva referencia, e incrementará refcount de tupla del argumento:
    tuplaResul = Py_BuildValue("iO", resultado.valor, tuplaKickers);
    Py_DECREF(tuplaKickers);
    return tuplaResul;
}

static PyObject *test(PyObject *self, PyObject *args, PyObject *kwargs)
{
    // Función inútil para hacer pruebas
    int nargs = 0, nkwargs = 0;
    char *keys[] = {"", "", "uno", "dos", "tres", NULL};
    int a = 0, b = 0, c = 0, d = 0, e = 0;
    if(args)
        nargs = PyTuple_Size(args);
    if(kwargs)
        nkwargs = PyDict_Size(kwargs);
    printf("%d %d\n", nargs, nkwargs);
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "iii|$ii", keys, &a, &b, &c, &d, &e))
    //if(!PyArg_ParseTuple(args, "ii|iii", &a, &b, &c, &d, &e))
    {
        PyErr_SetString(PyExc_ValueError, "Error parámetros");
        return NULL;
    }
    printf("%d %d %d %d %d\n", a, b, c, d, e);
    Py_RETURN_NONE;
}
// Definición del módulo Python ***********************************************

// Relación de funciones a exportar:
static PyMethodDef PokerlabFuns[] = {
    {"pl_valorMano", pl_valorMano, METH_VARARGS,
     "Comprueba el valor de una mano (5 o 7 cartas)"},
    {"pl_rangePercent", pl_rangePercent, METH_VARARGS,
     "Establece el rango actual a partir de un porcentaje (entero)"},
    {"pl_rangePrint", (PyCFunction)pl_rangePrint, METH_VARARGS | METH_KEYWORDS,
     "Muestra el rango actual en pantalla"},
    {"test", (PyCFunction)test, METH_VARARGS | METH_KEYWORDS,
     "Función inútil para hacer pruebas"},
    {NULL, NULL, 0, NULL}
};

// Definición del módulo:
static struct PyModuleDef pokerlab = {
    PyModuleDef_HEAD_INIT,
    "pokerlab",
    "Módulo con utilidades y cálculos relacionados con el póker",
    -1,
    PokerlabFuns
};

// Función de inicialización:
PyMODINIT_FUNC PyInit_pokerlab()
{
    PyObject *modulo;

    modulo = PyModule_Create(&pokerlab);
    return modulo;
}

