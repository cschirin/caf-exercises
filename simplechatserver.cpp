// C++ Actor Framework Exercises
// Christian Schirin, 2017-06-03 
// Exercise 3: Simple Chat (Server)

#include "caf/all.hpp"
#include "caf/io/all.hpp"

// "Implement an actor that receives join, chat, leave and ls messages."
// Pseudo Erlang notation: 
// {join, Name:string, Handle:strong_actor_ptr}
// {chat, Name:string, Msg:string} -> {chat, Name: string, Msg: string} // The server relays the messages to all the other actors
// {leave, Goodbye:string, OriginalSender:strong_actor_ptr} 
// down_msg are converted into leave messages
// {ls} -> NicknamesList: vector<string>


using namespace caf;

	// Atom type definitions.
	using chat_atom = atom_constant<atom("chat")>;
	using ls_atom = atom_constant<atom("ls")>;

	// Definition of the state of the simple chat server. 
	struct scs_state {
		// switched from two vectors to a map because 
		// two distinct participants may have the same name, but 
		// they can't have the same reference.
		std::map<actor, std::string> participants;
	};

	behavior scs_impl(stateful_actor<scs_state>* self) {
		// The actor should also monitor all chat participants and treat down messages in the same way leave messages are handled.
		self->set_down_handler([=](down_msg d) {
			// "Treat down messages in the same way leave messages are handled"
			// by converting them into leave messages and re-sending them!
			auto originalSender = actor_cast<actor>(self->current_sender());
			self->send(self, atom("leave"), to_string(d.reason), originalSender);
		});
		return {
			// "On a join message (that also contains a nickname and a handle to the new actor),
			// the sender of the message will be added to the list of chat participants."
			[=](join_atom, std::string name, actor handle) {
			// monitor the new participant
			self->monitor(handle);
			// add to nicknames
			self->state.participants[handle] = name;
			},
			// "On a chat message, the message is forwarded to all participants except for the sender of the chat message."
			[=](chat_atom, std::string name, std::string message) {
				auto sender = self->current_sender();
				for (auto participant : self->state.participants) {
					if (participant.first != sender) {
						// Cast the strong_actor_ptr to type scc.
						// https://actor-framework.readthedocs.io/en/stable/ReferenceCounting.html#actor-cast
						actor peer = actor_cast<actor>(participant.first);
						// Forward the message to the participant.
						self->send(peer,name,message);
					};
				}
			},
				// On leave message, (that may contain a goodbye message), the sender of the leave message is erased from the list of chat participants.
				[=](leave_atom, std::string goodbyeMessage, actor originalSender) {
				// Send the goodbye message
				self->send(self,atom("chat"),self->state.participants[originalSender],goodbyeMessage);
				// Remove from the list of participants
				self->state.participants.erase(originalSender);
			},
				// On an ls message, the actor responds with a list of known nicknames to the sender.
				[=](ls_atom) -> std::vector<std::string> {
					std::vector<std::string> result;
					// Get all the current participants and 
					// accumulate them in a list.
					for (auto participant : self->state.participants) {
						result.push_back(participant.second);
					}
					// Send the list of chat participants to the sender.
					return result;
				}
		};
	}

	// Helpful link: https://actor-framework.readthedocs.io/en/stable/ConfiguringActorApplications.html?highlight=actor_system_config
	class scs_config : public actor_system_config {
	public:
		uint16_t port = 0;
		scs_config() {
			// https://actor-framework.readthedocs.io/en/stable/NetworkTransparency.html#middleman
			// The middleman is the main component of the I/O module and enables distribution.
			opt_group{ custom_options_, "global" }
			.add(port, "port,p", "set port");
		}
	};

	// "Implement a main that publishes the implemented actor on the port specified in the commandline arguments."
	void caf_main(actor_system& system, const scs_config& cfg) {
		auto server = system.spawn(scs_impl);
		// Publish the implemented actor on the port specified in the command-line arguments.
		if (system.has_middleman()) {
			auto& mm = system.middleman();
			mm.publish(server, cfg.port);
		}
	}

CAF_MAIN(io::middleman)
