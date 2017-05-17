// C++ Actor Framework Exercices
// Christian Schirin, 2017-05-17
// Exercise 1: Polynomial Evaluator
#include <string>
#include <iostream>

#include "caf/all.hpp"

using std::endl;
using std::string;

using namespace caf;

// Type Aliases 
using calc_atom = atom_constant<atom("calc")>;

behavior polynomial(std::array<double, 5> as) {
	return {
		[=](const calc_atom& _calc, const double x) -> double {
		// Evaluate the polynomial.
		// The accumulator variable
		double result = 0;
		// x^4, x^3, x^2, x^1, x^0
		double exponent = as.size() - 1; // this is at generalized as it gets, it would also work for polynomials with a degree that is not 4.
		// We are doing this with a for loop because C++11 doesn't support fold
		for (double a : as) { 
			result += a * std::pow(x, exponent); //how does pow behave at x^0? // Apparently it returns 1.
			exponent -= 1;
		}
		return result;
	}
	};
}



int main() {
	// our CAF environment
	actor_system_config cfg;
	actor_system system{ cfg };

	std::array<double, 5> as;


	scoped_actor self{ system };

	// system will wait until both actors are destroyed before leaving main
	std::cout << std::pow(0, 0) << '\n';
}
