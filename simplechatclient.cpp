// C++ Actor Framework Exercises
// Christian Schirin, 2017-06-03 
// Exercise 3: Simple Chat (Server)

#include "caf/all.hpp"
#include "caf/io/all.hpp"

// The client is a simple command-line interface to the chat.
// On startup, the program asks the user for a nickname 
// that is used to register at the chat server.
// The chat program has the following interface:
behavior scc_impl(std::string name) {
	[=](std::string name, std::string message) {
		// "All received messages are printed to the terminal."
		std::cout << name << ": " << "message" << endl;
	},
	[=](std::vector<std::string> peers) {
		std::cout << "Connected peers: " << endl;
		for (auto peer : peers) {
			std::cout << peer << endl;
		}
	}

}


class scc_config : public actor_system_config {
	public:
		uint16_t port = 0;
		scs_config() {
			// https://actor-framework.readthedocs.io/en/stable/NetworkTransparency.html#middleman
			// The middleman is the main component of the I/O module and enables distribution.
			opt_group{ custom_options_, "global" }
			.add(port, "port,p", "set port");
		}
};

// "The client is a simple command-line interface to the chat."
void caf_main(actor_system& system, const scc_config& cfg) {
	auto client = system.spawn(scc_impl);
}

CAF_MAIN(io::middleman)
