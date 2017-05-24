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
		// Lambda expression, pass parameters by value
		[=](calc_atom, const double x) -> double {
		// Evaluate the polynomial.
		// The accumulator variable
		double result = 0;
		// x^4, x^3, x^2, x^1, x^0
		double exponent = as.size() - 1; // This is at generalized as it gets, it would also work for polynomials with a degree that is not 4.
		// Doing this with a for loop because C++11 doesn't support fold
		for (double a : as) { 
			result += a * std::pow(x, exponent); //how does pow behave at x^0? // Apparently it returns 1.
			exponent -= 1; // We need to go from size - 1 to 0 here.
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

	// Let us use an plain old for loop here because I don't see the benefit over using foreach here.
	// I do need the index here. Even if only to make the user prompt more understandable.
	for (unsigned int i = 0; i < as.size() ; i++) {
		// Prompt the user to input something
		std::cout << "Please input a" << i << ": ";
		// declare a temporary variable for the input
		string input;
		// read from cin
		std::cin >> input;
		//parse the doubles and write to the array
		as[i] = std::stod(input);
		// The obligatory newline
		std::cout << endl;
	}
	std::cout << "Thank you! I'll spawn the polynomial evaluator now!" << endl;

	scoped_actor self{ system };

	// Spawn the actor.
	// The signature of the spawn function doesn't look that straightforward, 
	// I think I'll go back to spawn/3 in Erlang, thanks. :-P
	auto p = self->spawn(polynomial, as); //using type inference because I do not know the type.

	// How would I even do this with the range based for loop?
	// I would need an iterator that returns something forever.
	// That is easy, but YAGNI, so a for (;;) will do.
	for (;;) {
		std::cout << "Please input an x: ";
		// declare a temporary variable for the input
		string input;
		// read from cin
		std::cin >> input;
		// parse the x 
		double x = stod(input);
		// send it to the actor 
		self->send(p, atom("calc"), x);

		//receive from the actor 
		self->receive(
			[&](double evaluatedPolynomial) {
			// I'd love to write unit tests for this here instead of writing it to aout.
			// Do you have something like eunit or common_test?
			std::cout << evaluatedPolynomial << endl;
		}
		);
	}


}
