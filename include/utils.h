#ifndef Utils_h_INCLUDED
#define Utils_h_INCLUDED

#include <string>
#include <vector>
#include <algorithm>	/* std::find, std::remove */
#include <functional>	/* std::function */
//#include <Python.h>

//#include "Group.h"
//#include "Map.h"

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
 * Calculate the minimum of two elements
 * @param a The first element
 * @param b The second element
 * @return The min
 */
template <typename T>
T min(T const& a, T const& b)
{
	return a < b ? a : b;
}

/**
 * Calculate the maximum of two elements
 * @param a The first element
 * @param b The second element
 * @return The max
 */
template <typename T>
T max(T const& a, T const& b)
{
	return a < b ? b : a;
}

/**
 * Calculate a binomial coefficient using Pascal triangle
 * @param n
 * @param k
 * @return The binomial coefficient
 */
int binomial_coef(int n, int k);

/**
 * Remove an element from a vector
 * @param vec The target vector
 * @param element The element to remove
 */
template <typename T>
bool find_element(std::vector<T> &vec, T element)
{
	return (std::find(vec.begin(), vec.end(), element) != vec.end());
}

/**
 * Remove an element from a vector
 * @param vec The target vector
 * @param element The element to remove
 */
template <typename T>
void remove_element(std::vector<T> &vec, T element)
{
	vec.erase(std::remove(vec.begin(), vec.end(), element), vec.end());
}

/**
 * Iterate through nested loops to execute a function
 * @param n The number of loops
 * @param maxes The maximum number of iterations of every loop
 * @param lambda The lambda function to execute for each last iteration
 */
void nested_loops(int n, int *maxes, std::function<void(int, int*)> &lambda);

/**
 * Convert a PyObject to a long integer
 * @param py_long The object to convert
 * @return The long integer or -1 if the conversion failed
 */
//long pyObject_to_long(PyObject *py_long);

/**
 * Convert a PyObject to a c string
 * @param py_str The object to convert
 * @return The c string
 */
//char* pyObject_to_cstring(PyObject *py_str);

/**
 * Convert a PyObject to a map
 * @param py_map The object to convert
 * @param map The map to store the converted object
 * @return If the conversion was a success
 */
//bool pyObject_to_map(PyObject *py_map, Map *&map);

#endif // Utils_h_INCLUDED
