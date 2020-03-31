#include "utils.h"

#include <stdint.h>		/* int16_t */
#include <stdlib.h>     /* atoi */
#include <sstream>
#include <cstring>      /* std::memset */
//#include <Python.h>

char* to_c_string(std::string str)
{
    char* cstr = new char[str.size() + 1];
    strcpy_s(cstr, sizeof(cstr), str.c_str());
    return cstr;
}

std::string int_to_string(int i)
{
    std::stringstream sstream;
    sstream << i;
    return sstream.str();
}

int binomial_coef(int n, int k)
{
    int *C = new int[k + 1];
    std::memset(C, 0, (k + 1) * sizeof(int));
    C[0] = 1;
    for (int i = 1; i <= n; ++i)
    {
        for (int j = min(i, k); j > 0; --j)
        {
            C[j] = C[j] + C[j - 1];
        }
    }
    int res = C[k];
    delete[] C;
    return res;
}

void nested_loops(int n, int *maxes, std::function<void(int, int*)> &lambda)
{
	int *indices = new int[n];
	std::memset(indices, 0, n * sizeof(int));

	int current = 0;
	while (indices[n - 1] < maxes[n - 1])
	{
		lambda(n, indices);
		++indices[current];
		while (indices[current] == maxes[current])
		{
			if (current == n - 1)
			{
				break;
			}
			indices[current] = 0;
			++indices[++current];
		}
		current = 0;
	}

	// Free memory
	delete[] indices;
}

/*long pyObject_to_long(PyObject *py_long)
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

char* pyObject_to_cstring(PyObject *py_str)
{
    if(!PyUnicode_CheckExact(py_str))
    {
        return "~E~";
    }
    PyObject *py_enc_str = PyUnicode_AsEncodedString(py_str, "utf-8", "~E~");
    return PyBytes_AS_STRING(py_enc_str);
}

bool pyObject_to_map(PyObject *py_map, Map *&map)
{
    // Check if the PyObject is a list
    if (!PyList_Check(py_map))
    {
        return false;
    }

    // Get its size
    const int n = PyList_GET_SIZE(py_map);
    if (n < 4 || n % 3 != 1)
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
    const int16_t lines = (int16_t) long_lines;
    const int16_t columns = (int16_t) long_columns;

    // Get player's species
    const char species = *pyObject_to_cstring(PyList_GET_ITEM(py_map, 2));

    // Get map implementation
    METHOD = (int16_t) pyObject_to_long(PyList_GET_ITEM(py_map, 3));

    // Initialize map
    switch (METHOD)
    {
        // MapVectors
        case 0:
            map = new MapVectors(lines, columns);
            break;

        // MapGrid
        case 1:
            map = new MapGrid(lines, columns);
            break;

        default:
            break;
    }

    // Get groups
    for (int i = 4; i < n; i += 3)
    {
        char *group = pyObject_to_cstring(PyList_GET_ITEM(py_map, i));
        if (strlen(group) < 2)
        {
            return false;
        }
        const char group_species = *group;
        char *group_number = strchr(group, group[1]);
        const int16_t number = (int16_t) atoi(group_number); // undefined behavior if not an int
        const long long_x = pyObject_to_long(PyList_GET_ITEM(py_map, i + 1));
        const long long_y = pyObject_to_long(PyList_GET_ITEM(py_map, i + 2));
        if (long_x < 0 || long_y < 0)
        {
            return false;
        }
        const int16_t x = (int16_t) long_x;
        const int16_t y = (int16_t) long_y;
        if (group_species == 'H')
        {
            map->add_human(x, y, number);
        }
        else if (group_species == species)
        {
            map->add_gentil(x, y, number);
        }
        else
        {
            map->add_vilain(x, y, number);
        }
    }

    return true;
}*/
