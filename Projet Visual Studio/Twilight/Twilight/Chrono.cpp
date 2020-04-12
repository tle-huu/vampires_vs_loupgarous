#include "Chrono.h"

double Chrono::get() const
{
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double>(end - start_).count();
}

void Chrono::reset()
{
	start_ = std::chrono::high_resolution_clock::now();
}

bool Chrono::alarm() const
{
	return *this > max_time_;
}

bool operator>(Chrono const& chrono, double duration)
{
	return chrono.get() > duration;
}

std::ostream& operator<<(std::ostream &out, Chrono const& chrono)
{
	out << chrono.get() << "s";
	return out;
}
