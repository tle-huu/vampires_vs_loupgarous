#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <Python.h>

#include "Map.h"

/**
 * Turn a std::string into a c string
 * @param str The string to convert
 * @return The string as a c string
 */
char* to_c_string(std::string str);

/**
 * Convert an int to a string
 * @param i The int to convert
 * @return The int as a string
 */
std::string int_to_string(int i);

/**
 * Convert a PyObject to a long integer
 * @param py_long The object to convert
 * @return The long integer or -1 if the conversion failed
 */
long pyObject_to_long(PyObject *py_long);

/**
 * Convert a PyObject to a c string
 * @param py_str The object to convert
 * @return The c string
 */
char* pyObject_to_cstring(PyObject *py_str);

/**
 * Convert a PyObject to a map
 * Deprecated
 * @param py_map The object to convert
 * @param map The map to store the converted object
 * @return If the conversion was a success
 */
bool pyObject_to_map(PyObject *py_map, std::vector<std::vector<char*> > *map);

/**
 * Convert a PyObject to a map
 * @param py_map The object to convert
 * @param map The map to store the converted object
 * @return If the conversion was a success
 */
bool pyObject_to_map(PyObject *py_map, Map *&map);

#endif // UTILS_H
