// C++ Actor Framework Exercices
// Christian Schirin, 2017-05-23
// Exercise 2: Type-safe Key-value Store

#include "caf/all.hpp"
#include <string>
#include <iostream>
#include <chrono>

using namespace caf;
using namespace std;

// Wait, wasn't "replies_to" called "responds_to" in the problem set?
using kvs = typed_actor<reacts_to<put_atom, uint32_t, std::string>,replies_to<get_atom, uint32_t>::with<std::string>>;

struct kvs_state {
	std::unordered_map<uint32_t, std::string> data;
};

kvs::behavior_type kvs_impl(kvs::stateful_pointer<kvs_state> self) {
	// Inspired by https://actor-framework.readthedocs.io/en/stable/Actors.html?highlight=replies_to#stateful-actors
	return {
		// Let's play it safe and pass by value
		[=](put_atom, const uint32_t key, const std::string value) {
			// Update the map
			self->state.data[key] = value;
		},
		[=](get_atom, const uint32_t key) -> std::string {
			// Retrieve from the map
			// The default initialization for std::string is the empty string http://en.cppreference.com/w/cpp/language/default_initialization
			// So this will respond with an empty string to unknown ids http://www.cplusplus.com/reference/unordered_map/unordered_map/operator[]/
			return self->state.data[key];
		}
	};

}


int main() {
	// our CAF environment
	actor_system_config cfg;
	actor_system system{ cfg };


	// Spawn one kvs_impl actor in your caf_main... 
	auto keyValueActor = system.spawn(kvs_impl);

	// and use a scoped_actor for communication.
	scoped_actor self{ system };

	// Change this on the user inputting "quit"
	bool running = true;

	// The main loop
	while (running) {
		std::string input;
		// use getline instead of >>
		std::getline(std::cin, input);
		std::cout << endl;
		std::vector<std::string> tokens;
		split(tokens, input, is_any_of(" "), token_compress_on);
		
		// put, get or quit
		std::string command = tokens[0];

		if (command == string("put")) {
			uint32_t key = stoi(tokens[1]);
			std::string value = tokens[2];
			self->request(keyValueActor, std::chrono::seconds(1), put_atom::value, key, value).receive([=](void) {
				std::cout << "Successfully inserted key " << tokens[1] << " with value " << value << " into the key-value store!" << endl;
			}, [](error) {});
		}
		else if (command == string("get")) {
			uint32_t key = stoi(tokens[1]);
			self->request(keyValueActor, std::chrono::seconds(1), get_atom::value, key).receive([=](std::string value) {
				std::cout << "Successfully retrieved key " << tokens[1] << " with value " << value << " from the key-value store!" << endl;
			}, [](error) {});
		}
		else if (command == string("quit")) { 
			running = false;
		}
		else {
			// Ignore the command.
		}
		// Write a newline 
		std::cout << endl;
	}
	std::cout << "Goodbye!" << endl;

}
