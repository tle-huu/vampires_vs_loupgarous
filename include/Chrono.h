#ifndef Chrono_h_INCLUDED
#define Chrono_h_INCLUDED

#include <chrono>
#include <float.h>
#include <iostream>

class Chrono
{
public:

	Chrono(double max_time = DBL_MAX) : start_(std::chrono::high_resolution_clock::now()), max_time_(max_time) {}

	double get() const;

	void reset();

	bool alarm() const;


private:

	std::chrono::high_resolution_clock::time_point start_;

	const double max_time_;
};

bool operator>(Chrono const& chrono, double duration);

std::ostream& operator<<(std::ostream &out, Chrono const& chrono);

#endif // Chrono_h_INCLUDED
