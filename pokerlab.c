// No podemos organizar el módulo en object files, porque todos los elementos
// deben tener linkage interno. Por lo tanto solo podemos tener una translation
// unit (no procede Makefile).

#include "pokerlab.h"

// Definición de funciones "privadas" *****************************************

#include "utils.c"
#include "simulations.c"
#include "rangeparse.c"
#include "ranges.c"

// Definición de funciones a exportar *****************************************

static PyObject *simHandVsHand(PyObject *self, PyObject *args)
{
    // Retorna tupla con datos de (win, lose, tie) a partir de dos pares (el del
    // hero y el del villain) en 4 argumentos (cartas: h1, h2, v1, v2) con formato
    // entero (0-51) o string (tipo 'Td').
    struct Carta cartas[4];
    struct WLS resultado;
    PyObject *resul;
    int i;
    PyObject *arg;
    if(PySequence_Length(args) != 4)
    {
        PyErr_SetString(PyExc_TypeError, "Se esperan 4 argumentos.");
        return NULL;
    }
    // Vamos a recoger argumentos y  almacenar las cartas en el formato correcto:
    for(i = 0; i < 4; i++)
    {
        arg = PyTuple_GetItem(args, i);    // borrowed reference
        if(PyLong_Check(arg))
            cartas[i] = _intToCard(PyLong_AsLong(arg));
        else if(PyUnicode_Check(arg))
            cartas[i] = _strToCard(arg);
        else
        {
            PyErr_SetString(PyExc_TypeError, "Argumento debe ser entero o string.");
            return NULL;
        }
        if(cartas[i].rank == -1)
        {
            PyErr_SetString(PyExc_ValueError, "Argumento debe ser 0-51 o tipo 'Td'.");
            return NULL;
        }
    }
    // Ya tenemos las cartas en 'cartas'
    if(_cartasRepetidas(cartas, 4))  // ¿alguna repetida?
    {
        PyErr_SetString(PyExc_ValueError, "Las cuatro cartas deben ser distintas.");
        return NULL;
    }
    resultado = _simHandVsHand(cartas);
    resul = Py_BuildValue("iii", resultado.win,
        resultado.lose, resultado.tie);  // new reference
    return resul;    // pasamos ownership
}

static PyObject *simHandVsRange(PyObject *self, PyObject *args)
{
    // Retorna tupla con datos (win, lose, tie) a partir de un par de cartas:
    // dos argumentos de tipo entero (0-51) o string (tipo 'Td'). Un tercer argumento
    // indica el range con el que contrastar: False (default, actual del hero),
    // True (actual del villano) o un string especificando el range. No se
    // modifican los rangos actuales.
    int i;
    struct WLS resultado;
    PyObject *o, *arg3 = Py_False;  // no refcount++ Py_False (var local)
    struct Carta cartas[2];
    _Bool (*r)[13];
    int nargs = PySequence_Length(args);
    if(nargs != 2 && nargs != 3)
    {
        PyErr_SetString(PyExc_TypeError, "Se esperan 2 o 3 argumentos.");
        return NULL;
    }
    // Vamos a recoger las 2 cartas:
    for(i = 0; i < 2; i++)
    {
        o = PyTuple_GetItem(args, i);    // borrowed reference
        if(PyLong_Check(o))
            cartas[i] = _intToCard(PyLong_AsLong(o));
        else if(PyUnicode_Check(o))
            cartas[i] = _strToCard(o);
        else
        {
            PyErr_SetString(PyExc_TypeError, "Cartas deben ser entero o string.");
            return NULL;
        }
        if(cartas[i].rank == -1)
        {
            PyErr_SetString(PyExc_ValueError, "Cartas deben ser 0-51 o tipo 'Td'.");
            return NULL;
        }
    }
    // Ya tenemos las cartas en 'cartas'
    if(cartas[0].rank == cartas[1].rank && cartas[0].suit == cartas[1].suit)
    {
        PyErr_SetString(PyExc_ValueError, "Las cartas deben ser distintas.");
        return NULL;
    }
    // Ahora, a por el range:
    if(nargs == 3)
        arg3 = PyTuple_GetItem(args, 2);
    if(arg3 == Py_True)
        r = _rangeVillain;
    else if(arg3 == Py_False)
        r = _rangeHero;
    else if(PyUnicode_Check(arg3))
    {
        _rangeClear(_rangeTMP);
        if(!_rangeParse(PyUnicode_AsUTF8(arg3), 1))
        {
            PyErr_SetString(PyExc_ValueError,
                "Formato incorrecto.");
            return NULL;
        }
        r = _rangeTMP;
    }
    else
    {
        PyErr_SetString(PyExc_TypeError,
            "Tercer argumento (opcional) debe ser booleano o string.");
        return NULL;
    }
    resultado = _simHandVsRange(cartas, r);
    o = Py_BuildValue("iii", resultado.win,
        resultado.lose, resultado.tie);  // new reference
    return o;    // pasamos ownership
}

static PyObject *simRangeVsRange(PyObject *self, PyObject *args)
{
    // Retorna tupla con datos de (win, lose, tie) de un range contra otro.
    // Sin argumentos, usa los ranges actuales de hero y villain.
    // Con dos strings, usa los dos rangos especificados (no se tocan los actuales).
    struct WLS resultado;
    PyObject *resul;
    const char *str1, *str2;
    int nargs = PySequence_Length(args);
    if(nargs == 0)
        resultado = _simRangeVsRange(_rangeHero, _rangeVillain);
    else if(nargs == 2)
    {
        if(!PyArg_ParseTuple(args, "ss", &str1, &str2))
        {
            PyErr_SetString(PyExc_TypeError, "Se esperan 2 strings.");
            return NULL;
        }
        _rangeClear(_rangeTMP);
        if(!_rangeParse(str2, 1))
        {
            PyErr_SetString(PyExc_ValueError,
                "Formato incorrecto.");
            return NULL;
        }
        _rangeCopy(_rangeTMP, _rangeTMP2);
        _rangeClear(_rangeTMP);
        if(!_rangeParse(str1, 1))
        {
            PyErr_SetString(PyExc_ValueError,
                "Formato incorrecto.");
            return NULL;
        }
        resultado = _simRangeVsRange(_rangeTMP, _rangeTMP2);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Se esperan 0 o 2 argumentos.");
        return NULL;
    }
    resul = Py_BuildValue("KKK", resultado.win,
        resultado.lose, resultado.tie);  // new reference
    return resul;    // pasamos ownership
}

static PyObject *rangeSet(PyObject *self, PyObject *args, PyObject *kwargs)
{
    // Establece tabla preflop (hero o villain) a partir de un string
    // que define el range.
    // Argumentos desde Python: (action='set', range='', villain=False)
    // posicionales o keyword.
    char *keys[] = {"action", "range", "villain", NULL};
    char *action = "set", *range = "";
    PyObject *villain = Py_False;
    _Bool (*r)[13], parseOK = 1;
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|ssO", keys,
        &action, &range, &villain))
    {
        PyErr_SetString(PyExc_TypeError,
            "Argumentos esperados (opcionales): "
            "action(string), range(string), villain(boolean).");
        return NULL;
    }
    if(!PyBool_Check(villain))
    {
        PyErr_SetString(PyExc_TypeError,
            "Argumento 'villain' debe ser booleano");
        return NULL;
    }
    if(villain == Py_True)
        r = _rangeVillain;
    else
        r = _rangeHero;
    if(!strcmp(action, "clear"))
    {
        _rangeClear(r);
        return _rangeGetString(r);
    }
    else if(!strcmp(action, "invert"))
    {
        _rangeInvert(r);
        return _rangeGetString(r);
    }
    if(!strcmp(action, "set"))
    {
        _rangeClear(_rangeTMP);
        parseOK = _rangeParse(range, 1);
    }
    else if(!strcmp(action, "add"))
    {
        _rangeCopy(r, _rangeTMP);
        parseOK = _rangeParse(range, 1);
    }
    else if(!strcmp(action, "subtract"))
    {
        _rangeCopy(r, _rangeTMP);
        parseOK = _rangeParse(range, 0);
    }
    else
    {
        PyErr_SetString(PyExc_ValueError,
            "Acciones: 'set' (default), 'add', 'subtract', 'clear', 'invert'.");
        return NULL;
    }
    if(!parseOK)
    {
        PyErr_SetString(PyExc_ValueError,
            "Formato incorrecto.");
        return NULL;
    }
    _rangeCopy(_rangeTMP, r);
    return _rangeGetString(r);
}

static PyObject *rangeGetPercent(PyObject *self, PyObject *args)
{
    // Retorna el porcentaje (float) que compone el range actual (de hero o villain).
    // Argumentos desde Python: un booleano posicional opcional (por defecto False,
    // indicando hero).
    PyObject *villain = NULL, *resul;
    _Bool (*r)[13] = _rangeHero;
    if(!PyArg_ParseTuple(args, "|O", &villain))
    {
        PyErr_SetString(PyExc_TypeError,
            "Se esperaba 1 argumento posicional (opcional)");
        return NULL;
    }
    if(villain)
    {
        if(!PyBool_Check(villain))
        {
            PyErr_SetString(PyExc_TypeError,
                "Argumento debe ser booleano");
            return NULL;
        }
        if(villain == Py_True)
            r = _rangeVillain;
    }
    resul = PyFloat_FromDouble(_rangeGetPercent(r));
    // La new reference obtenida se retorna a Python (se pasa ownership):
    return resul;
}

static PyObject *rangeGet(PyObject *self, PyObject *args, PyObject *kwargs)
{
    // Retorna en un string el range actual (hero o villain), o la
    // tabla preflop en una tupla.
    // Argumentos desde Python: (table=False, villain=False),
    // posicionales o keyword.
    char *keys[] = {"table", "villain", NULL};
    PyObject *table = Py_False, *villain = Py_False;
    _Bool (*r)[13];
    if(!PyArg_ParseTupleAndKeywords(args, kwargs, "|OO", keys,
        &table, &villain))
    {
        PyErr_SetString(PyExc_TypeError,
            "Argumentos esperados (opcionales): "
            "table(boolean), villain(boolean).");
        return NULL;
    }
    if(!PyBool_Check(villain) || !PyBool_Check(table))
    {
        PyErr_SetString(PyExc_TypeError,
            "Argumentos deben ser booleanos.");
        return NULL;
    }
    if(villain == Py_True)
        r = _rangeVillain;
    else
        r = _rangeHero;
    if(table == Py_True)
        return _rangeGetTable(r);
    else
        return _rangeGetString(r);
}

static PyObject *rangeShow(PyObject *self, PyObject *args)
{
    // Muestra tabla de range actual (de hero o villain).
    // Argumentos desde Python: un booleano posicional opcional, por defecto
    // False (indicando hero).
    PyObject *villain = NULL;
    _Bool (*r)[13] = _rangeHero;
    if(!PyArg_ParseTuple(args, "|O", &villain))
    {
        PyErr_SetString(PyExc_TypeError,
            "Se esperaba 1 argumento posicional (opcional)");
        return NULL;
    }
    if(villain)
    {
        if(!PyBool_Check(villain))
        {
            PyErr_SetString(PyExc_TypeError,
                "Argumento debe ser booleano");
            return NULL;
        }
        if(villain == Py_True)
            r = _rangeVillain;
    }
    _rangePrintTable(r);
    Py_RETURN_NONE;
}

static PyObject *valorMano(PyObject *self, PyObject *args)
{
    // Retorna valor de la mano en una tupla (figura + kickers).
    // Argumentos desde Python: 5 o 7 cartas de tipo entero (0-51) o
    // string ('AKs', 'JT',...). Posicionales.
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
    if(_cartasRepetidas(cartas, numargs))
    {
        PyErr_SetString(PyExc_ValueError, "Las cartas deben ser distintas entre sí.");
        return NULL;
    }
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
    {"simHandVsHand", simHandVsHand, METH_VARARGS,
     "Retorna tupla con datos win, lose, tie de mano contra mano"},
    {"simHandVsRange", simHandVsRange, METH_VARARGS,
     "Retorna tupla con datos win, lose, tie de mano contra un range"},
    {"simRangeVsRange", simRangeVsRange, METH_VARARGS,
     "Retorna tupla con datos win, lose, tie de range contra un range"},
    {"rangeSet", (PyCFunction)rangeSet, METH_VARARGS | METH_KEYWORDS,
     "Establece el rango actual (de hero o villain) a partir de un string"},
    {"rangeGetPercent", rangeGetPercent, METH_VARARGS,
     "Retorna porcentaje del rango actual (de hero o villain) en un float"},
    {"rangeGet", (PyCFunction)rangeGet, METH_VARARGS | METH_KEYWORDS,
     "Retorna string o tupla que define el rango actual (de hero o villain)"},
    {"rangeShow", rangeShow, METH_VARARGS,
     "Muestra el rango actual (de hero o villain) en una tabla"},
    {"valorMano", valorMano, METH_VARARGS,
     "Comprueba el valor de una mano (5 o 7 cartas) y retorna tupla descriptiva"},
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

