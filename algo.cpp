#include <stdint.h>     /* (u)intN_t */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <Python.h>

#include "include/utils.h"
#include "include/Point.h"
#include "include/Group.h"
#include "include/Map.h"
#include "include/Action.h"
#include "include/State.h"
#include "include/minimax.h"

/* Maximum depth of the game tree to explore */
static const uint8_t DEPTH = 3;

/**
 * Calculate the best action to do in a given state
 * @param map The current state of the game
 * @param counter (For testing purposes only)
 * @return The best action found as a string
 */
char* get_move(Map *map, int counter)
{
    // Create initial state
    State initial_state(*map);

    // Build tree
    build_tree(initial_state, DEPTH);

    // Get action
    Action action = minimax(initial_state);

    // For testing only
    if (counter == 0)
    {
        Move move(5, 4, 3, 4, 4);
        action.add_move(move);
    }
    else if (counter == 1)
    {
        Move move(4, 4, 4, 3, 3);
        action.add_move(move);
    }
    else if (counter == 2)
    {
        Move move(3, 3, 4, 2, 2);
        action.add_move(move);
    }
    else
    {
        Move move(2, 2, 5, 2, 3);
        action.add_move(move);
    }

    // Delete map
    delete map;

    // Return action as a c string
    return action.to_cstring();
}


/* LINK TO PYTHON LIBRARY */

// Encapsulation of 'get_move' function for Python
static PyObject* algo_get_move(PyObject *self, PyObject *args)
{
    PyObject *py_map;
    int counter;
    if (!PyArg_ParseTuple(args, "Oi", &py_map, &counter))
    {
        return NULL;
    }
    Map *map = 0;
    if (!pyObject_to_map(py_map, map))
    {
        return NULL;
    }
    return (PyObject*) Py_BuildValue("s", get_move(map, counter));
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
