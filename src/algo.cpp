#include <stdint.h>     /* int16_t */
#include <iostream>
#include <Python.h>

#include "../include/utils.h"
#include "../include/Map.h"
#include "../include/Action.h"
#include "../include/State.h"
#include "../include/minimax.h"

// Maximum depth of the game tree to explore
static const int16_t MAX_DEPTH = 3;

/**
 * Calculate the best action to do in a given state
 * @param map The current state of the game
 * @return The best action found as a string
 */
char* get_move(Map *map)
{
    // Create initial state
    State origin(map);

    // Get action
    Action action = expect_minimax_alphabeta(&origin, MAX_DEPTH);

    // Return action as a c string
    return action.to_cstring();
}


/* LINK TO PYTHON LIBRARY */

// Encapsulation of 'get_move' function for Python
static PyObject* algo_get_move(PyObject *self, PyObject *args)
{
    PyObject *py_map;
    if (!PyArg_ParseTuple(args, "O", &py_map))
    {
        return NULL;
    }
    Map *map;
    if (!pyObject_to_map(py_map, map))
    {
        return NULL;
    }
    return (PyObject*) Py_BuildValue("s", get_move(map));
}

// List of objects available in the module
static PyMethodDef module_methods[] = 
{
    {"get_move", (PyCFunction) algo_get_move, METH_VARARGS},
    {NULL, NULL}
};

// Structure for module initialization
static struct PyModuleDef cModPyDem =
{
    PyModuleDef_HEAD_INIT,
    "algo",                                                         /* name of module */
    "Package to have an IA playing Vampires versus Werewolves",     /* module documentation, may be NULL */
    -1,                                                             /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables */
    module_methods
};

// Module initialization by Python during import
PyMODINIT_FUNC PyInit_algo(void)
{
    return PyModule_Create(&cModPyDem);
}
