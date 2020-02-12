#include <stdint.h>     /* uint8_t, uint16_t */
#include <stdlib.h>     /* atoi */
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>    /* max */
#include <Python.h>

class Point
{
public:
    Point(uint8_t x, uint8_t y) : m_x(x), m_y(y) {}

    uint8_t x() const { return m_x; }
    uint8_t y() const { return m_y; }

private:
    const uint8_t m_x;
    const uint8_t m_y;
};

uint16_t distance(Point const& point1, Point const& point2)
{
    return std::max(abs(point1.x() - point2.x()), abs(point1.y() - point2.y()));
}

class Group
{
public:
    Group(Point position, uint16_t number) : m_position(position), m_number(number) {}

    Point position() const { return m_position; }
    uint16_t number() const { return m_number; }

protected:
    Point m_position;
    uint16_t m_number;
};

uint16_t distance(Group const& group1, Group const& group2)
{
    return distance(group1.position(), group2.position());
}

class Gentil : public Group
{
public:
    Gentil(Point position, uint16_t number) : Group(position, number) {}
};

class Vilain : public Group
{
public:
    Vilain(Point position, uint16_t number) : Group(position, number) {}
};

class Human : public Group
{
public:
    Human(Point position, uint16_t number) : Group(position, number) {}
};

class Map
{
public:
    Map(uint8_t lines, uint8_t columns) : m_lines(lines), m_columns(columns) {}

    uint8_t lines() const { return m_lines; }
    uint8_t columns() const { return m_columns; }

    void add_gentil(Gentil gentil) { m_gentils.push_back(gentil); }
    void add_vilain(Vilain vilain) { m_vilains.push_back(vilain); }
    void add_human(Human human) { m_humans.push_back(human); }

private:
    const uint8_t m_lines;
    const uint8_t m_columns;
    std::vector<Gentil> m_gentils;
    std::vector<Vilain> m_vilains;
    std::vector<Human> m_humans;
};

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
 * Convert a PyObject to a long integer
 * @param py_long The object to convert
 * @return The long integer or -1 if the conversion failed
 */
long pyObject_to_long(PyObject *py_long)
{
    if (!PyLong_Check(py_long))
    {
        return -1;
    }
    const long i = PyLong_AsLong(py_long);
    if (i < 0)
    {
        return -1;
    }
    return i;
}

/**
 * Convert a PyObject to a c string
 * @param py_str The object to convert
 * @return The c string
 */
char* pyObject_to_cstring(PyObject *py_str)
{
    if(!PyUnicode_CheckExact(py_str))
    {
        return "~E~";
    }
    PyObject *py_enc_str = PyUnicode_AsEncodedString(py_str, "utf-8", "~E~");
    return PyBytes_AS_STRING(py_enc_str);
}

/**
 * Convert a PyObject to a map
 * Deprecated
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
    const int n = PyList_GET_SIZE(py_map);
    for (int i = 0; i < n; ++i)
    {
        const PyObject *py_line = PyList_GET_ITEM(py_map, i);
        if (!PyList_Check(py_line))
        {
            return false;
        }
        const int m = PyList_GET_SIZE(py_line);
        std::vector<char*> line(m);
        for (int j = 0; j < m; ++j)
        {
            line[j] = pyObject_to_cstring(PyList_GET_ITEM(py_line, j));
        }
        map->push_back(line);
    }
    return true;
}

/**
 * Convert a PyObject to a map
 * @param py_map The object to convert
 * @param map The map to store the converted object
 * @return If the conversion was a success
 */
bool pyObject_to_map(PyObject *py_map, Map *map)
{
    // Check if the PyObject is a list
    if (!PyList_Check(py_map))
    {
        return false;
    }

    // Get its size
    const int n = PyList_GET_SIZE(py_map);
    if (n < 2 || n % 3 != 0)
    {
        return false;
    }

    // Get map dimensions
    const long long_lines = pyObject_to_long(PyList_GET_ITEM(py_map, 0));
    const long long_columns = pyObject_to_long(PyList_GET_ITEM(py_map, 1));
    if (long_lines < 0 || long_columns < 0)
    {
        return false;
    }
    const uint8_t lines = (uint8_t) long_lines;
    const uint8_t columns = (uint8_t) long_columns;
    map = new Map(lines, columns);

    // Get player's species
    const char species = *pyObject_to_cstring(PyList_GET_ITEM(py_map, 2));

    // Get groups
    for (int i = 3; i < n; i += 3)
    {
        char *group = pyObject_to_cstring(PyList_GET_ITEM(py_map, i));
        if (strlen(group) < 2)
        {
            return false;
        }
        const char group_species = *group;
        char *group_number = strchr(group, group[1]);
        const uint16_t number = (uint16_t) atoi(group_number); // undefined behavior if not an int
        const long long_x = pyObject_to_long(PyList_GET_ITEM(py_map, i + 1));
        const long long_y = pyObject_to_long(PyList_GET_ITEM(py_map, i + 2));
        if (long_x < 0 || long_y < 0)
        {
            return false;
        }
        const uint8_t x = (uint8_t) long_x;
        const uint8_t y = (uint8_t) long_y;
        if (group_species == 'H')
        {
            map->add_human(Human(Point(x, y), number));
        }
        else if (group_species == species)
        {
            map->add_gentil(Gentil(Point(x, y), number));
        }
        else
        {
            map->add_vilain(Vilain(Point(x, y), number));
        }
    }

    return true;
}

/**
 * Calculate the best move to do in a given state
 * @param map The current state of the game
 * @param counter (For testing purposes only)
 * @return The best move found as a string
 */
char* get_move(Map *map, int counter)
{
    // Initialize move
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

    // Delete map
    delete map;

    // Return move
    return to_c_string(move);
}

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
