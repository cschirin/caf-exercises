// C++ Actor Framework Exercises
// Christian Schirin, 2017-06-03 
// Exercise 3: Simple Chat (Server)

#include "caf/all.hpp"
#include "caf/io/all.hpp"
#include <iostream>

using namespace std;
using namespace caf;

using chat_atom = atom_constant < atom("chat")>;
using ls_atom = atom_constant<atom("ls")>;

// The client is a simple command-line interface to the chat.
// On startup, the program asks the user for a nickname 
// that is used to register at the chat server.
// The chat program has the following interface:
behavior scc_impl(event_based_actor* self, const actor server) {
    return {
        // Messages coming from the scoped actor in the client main
        [=](join_atom, std::string name) {
            self->send(server, atom("join"),name, self);
        },
        [=](chat_atom, std::string name, std::string message) {
            self->send(server, atom("chat"), name, message);
        },
        [=](leave_atom, std::string goodbyeMessage) {
            self->send(server, atom("leave"), goodbyeMessage, self);
        },
        [=](ls_atom) {
            self->send(server, atom("ls"));
        },
        // Messages coming from the server
        [=](std::string name, std::string message) {
            // "All received messages are printed to the terminal."
            std::cout << name << ": " << message << endl;
        },
        [=](std::vector<std::string> peers) {
            std::cout << "Connected peers: " << std::endl;
            for (auto peer : peers) {
                std::cout << peer << endl;
            };
        }
    };
}


class scc_config : public actor_system_config {
	public:
		uint16_t port = 0;
		std::string name; 
        bool running = true;
		scc_config() {
			// https://actor-framework.readthedocs.io/en/stable/NetworkTransparency.html#middleman
			// The middleman is the main component of the I/O module and enables distribution.
			opt_group{ custom_options_, "global" }
			.add(port, "port,p", "set port")
			.add(name, "name,n", "set nickname");
		}
};

// "The client is a simple command-line interface to the chat."
void caf_main(actor_system& system, const scc_config& cfg) {
// https://github.com/actor-framework/actor-framework/blob/master/examples/remoting/group_chat.cpp#L95
    auto name = cfg.name;
    while (name.empty()) {
      cout << "please enter your name: " << flush;
      if (!getline(cin, name)) {
        cerr << "*** no name given... terminating" << endl;
        return;
      }
    }
    
    // Join the server.
// https://actor-framework.readthedocs.io/en/stable/NetworkTransparency.html#publishing-and-connecting
    auto server = system.middleman().remote_actor("127.0.0.1", cfg.port);
    if (!server) {
        cerr << "Unable to connect to the chat server. "
            << "Error message: "
            << system.render(server.error()) << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Connecting to the chat server on Port " << cfg.port << endl;
    // The client can be spawned now.
    // It is safe to "dereference" server now.
    actor client = system.spawn(scc_impl, *server);
    while (cfg.running) {
        // Prompt the user.
        cout << name << ">" << endl;
        // Process the input.
        // Read the first character.
        string line;
        if (line[0] == '/') {
            // Process the /ls or /quit command.
            if (line[1] == 'l' && line[2] == 's') {
                // ls message
                
            }
            else if (line[1] == 'q' && line[2] == 'u' && 
                     line[3] == 'i' && line[4] == 't')
                // quit message  
            }
            else {
                std::cout << "Please input a valid command."
                    << "\ls \quit or a chat message. ";
            }
        } else {
            // Process the chat message.
      
        }
    };

}

CAF_MAIN(io::middleman)
