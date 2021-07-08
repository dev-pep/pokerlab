#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "utils.h"

// Definición de funciones a exportar *****************************************

static PyObject *pl_valorMano(PyObject *self, PyObject *args)
{
    int i, j;
    struct Carta cartas[7], c;
    struct Valor resultado;
    PyObject *tuplaKickers, *tuplaResul;
    Py_ssize_t numargs = PySequence_Length(args);

    if((numargs != 5) && (numargs != 7))
    {
        PyErr_SetString(PyExc_TypeError, "La función Pl_hand() espera 5 o 7 argumentos");
        return NULL;
    }
    // Vamos a recoger argumentos y  almacenar las cartas en el formato correcto:
    PyObject *arg;
    for(i = 0; i < numargs; i++)
    {
        arg = PyTuple_GetItem(args, i);    // borrowed reference
        if(PyLong_Check(arg))
            c = _pl_intToCard(PyLong_AsLong(arg));
        else if(PyUnicode_Check(arg))
            c = _pl_strToCard(arg);
        else
        {
            PyErr_SetString(PyExc_TypeError, "Argumento debe ser entero o string");
            return NULL;
        }
        if(c.rank == -1)
        {
            PyErr_SetString(PyExc_ValueError, "Argumento debe ser 0-51 o tipo 'Td'");
            return NULL;
        }
        cartas[i] = c;
    }
    // Ya tenemos las cartas en 'cartas'
    resultado = _pl_valorMano(cartas, numargs);
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

// Fin de las funciones a exportar ********************************************

// Relación de funciones a exportar:
static PyMethodDef PokerlabFuns[] = {
    {"pl_valorMano", pl_valorMano, METH_VARARGS, "Comprueba el valor de una mano (5 o 7 cartas)"},
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

