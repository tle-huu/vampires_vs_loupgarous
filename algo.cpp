#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <Python.h>
  
int fact(int n)
{
    if (n < 2)
    {
        return 1;
    }
    return n * fact(n - 1);
}

// Encapsulation de la fonction 'fact' pour utilisation par Python
static PyObject* algo_fact(PyObject *self, PyObject *args)
{
    int n;
    if (!PyArg_ParseTuple(args, "i", &n))
    {
        return NULL;
    }
    return (PyObject*) Py_BuildValue("i", fact(n));
}


char* to_c_string(std::string s)
{
    char* cstr = new char[s.size() + 1];
    strcpy(cstr, s.c_str());
    return cstr;
}

std::string int_to_string(int i)
{
    std::stringstream sstream;
    sstream << i;
    return sstream.str();
}

std::string int_to_hex_string(int i) // 0 <= i < 256
{
    std::stringstream sstream;
    if (i < 16)
    {
        sstream << "x0" << std::hex << i;
        return sstream.str();
    }
    else
    {
        sstream << "x" << std::hex << i;
        return sstream.str();
    }
}

char* get_move(/*std::vector<std::vector<std::string> > map, */int counter)
{
    std::string move = "";
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

// Encapsulation de la fonction 'get_move' pour utilisation par Python
static PyObject* algo_get_move(PyObject *self, PyObject *args)
{
    //PyObject *map;
    int counter;
    //if (!PyArg_ParseTuple(args, "iO", &counter, &map))
    if (!PyArg_ParseTuple(args, "i", &counter))
    {
        return NULL;
    }
    return (PyObject*) Py_BuildValue("s", get_move(/*map, */counter));
}

// Liste des objets disponibles du module
static PyMethodDef module_methods[] = 
{
    {"fact", (PyCFunction) algo_fact, METH_VARARGS},
    {"get_move", (PyCFunction) algo_get_move, METH_VARARGS},
    {NULL, NULL}
};

// Création d'une structure pour l'initialisation du module
static struct PyModuleDef cModPyDem =
{
    PyModuleDef_HEAD_INIT,
    "algo", /* name of module */
    "",     /* module documentation, may be NULL */
    -1,     /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables */
    module_methods
};

// Initialisation du module par Python pendant l'importation
PyMODINIT_FUNC PyInit_algo(void)
{
    return PyModule_Create(&cModPyDem);
}
