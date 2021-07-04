#define PY_SSIZE_T_CLEAN

#include <Python.h>

// Definición de funciones a exportar *****************************************

static PyObject *Pl_hand(PyObject *self, PyObject *args)
{
    printf("Hello, world!\n");
    return PyLong_FromLong(0);
}

// Fin de las funciones a exportar ********************************************

// Relación de funciones a exportar:
static PyMethodDef PokerlabFuns[] = {
    {"hand", Pl_hand, METH_VARARGS, "Comprueba el valor de una mano (5 o 7 cartas)"},
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

