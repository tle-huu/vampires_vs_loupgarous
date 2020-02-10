#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <Python.h>

/**
 * Turn a std::string into a c string
 * @param str The string to convert
 * @return The string as a c string
 */
char* to_c_string(std::string str)
{
    char* cstr = new char[str.size() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}

/**
 * Convert an int to a string
 * @param i The int to convert
 * @return The int as a string
 */
std::string int_to_string(int i)
{
    std::stringstream sstream;
    sstream << i;
    return sstream.str();
}

/**
 * Convert a PyObject to a map
 * @param py_map The object to convert
 * @param map The map to store the converted object
 * @return If the conversion was a success
 */
bool pyObject_to_map(PyObject *py_map, std::vector<std::vector<char*> > *map)
{
    if (!PyList_Check(py_map))
    {
        return false;
    }
    int n = PyList_GET_SIZE(py_map);
    for (int i = 0; i < n; ++i)
    {
        PyObject *py_line = PyList_GET_ITEM(py_map, i);
        if (!PyList_Check(py_line))
        {
            return false;
        }
        int m = PyList_GET_SIZE(py_line);
        std::vector<char*> line(m);
        for (int j = 0; j < m; ++j)
        {
            PyObject *py_val = PyList_GET_ITEM(py_line, j);
            PyObject *py_repr = PyObject_Repr(py_val);
            PyObject *py_str = PyUnicode_AsEncodedString(py_repr, "utf-8", "~E~");
            char *val = PyBytes_AS_STRING(py_str);
            line[j] = val;
        }
        map->push_back(line);
    }
    return true;
}

/**
 * Calculate the best move to do in a given state
 * @param species The player's species ('V' or 'W')
 * @param map The current state
 * @param counter For testing purposes only
 * @return The best move found as a string
 */
char* get_move(char species, std::vector<std::vector<char*> > map, int counter)
{
    std::string move = "";

    // For testing only
    if (counter == 0)
    {
        move = int_to_string(1) + "x" + int_to_string(5) + "x" + int_to_string(4) + "x" + int_to_string(3) + "x" + int_to_string(4) + "x" + int_to_string(4) + "x";
    }
    else if (counter == 1)
    {
        move = int_to_string(1) + "x" + int_to_string(4) + "x" + int_to_string(4) + "x" + int_to_string(4) + "x" + int_to_string(3) + "x" + int_to_string(3) + "x";
    }
    else if (counter == 2)
    {
        move = int_to_string(1) + "x" + int_to_string(3) + "x" + int_to_string(3) + "x" + int_to_string(4) + "x" + int_to_string(2) + "x" + int_to_string(2) + "x";
    }
    else
    {
        move = int_to_string(1) + "x" + int_to_string(2) + "x" + int_to_string(2) + "x" + int_to_string(5) + "x" + int_to_string(2) + "x" + int_to_string(3) + "x";
    }

    return to_c_string(move);
}

// Encapsulation of 'get_move' function for Python
static PyObject* algo_get_move(PyObject *self, PyObject *args)
{
    char *species;
    PyObject *py_map;
    int counter;
    if (!PyArg_ParseTuple(args, "sOi", &species, &py_map, &counter))
    {
        return NULL;
    }
    std::vector<std::vector<char*> > map;
    if (!pyObject_to_map(py_map, &map))
    {
        return false;
    }
    return (PyObject*) Py_BuildValue("s", get_move(*species, map, counter));
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
    "algo", /* name of module */
    "",     /* module documentation, may be NULL */
    -1,     /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables */
    module_methods
};

// Module initialization by Python during import
PyMODINIT_FUNC PyInit_algo(void)
{
    return PyModule_Create(&cModPyDem);
}
