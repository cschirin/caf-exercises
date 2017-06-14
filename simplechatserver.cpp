// C++ Actor Framework Exercises
// Christian Schirin, 2017-06-03 
// Exercise 3: Simple Chat (Server)

#include "caf/all.hpp"

// "Implement an actor that receives join, chat, leave and ls messages.
// Pseudo Erlang notation: 
// {join, Name:string, Handle}
// {chat, Msg:string} -> {chat, Message: string} // The server just relays the messages to all the other actors
// {leave, Goodbye:string, Handle} 
// down_msg are converted into leave messages
// {ls} -> NicknamesList: vector<string>

using namespace caf;

// Atom type definitions.
using chat_atom = atom_constant<atom("chat")>;
using ls_atom = atom_constant<atom("ls")>;

// Type definition of the simple chat client.
using scc = typed_actor<
	reacts_to<std::string>,
	reacts_to<std::vector<std::string>>
>;

// Type definition of the simple chat server.
using scs = typed_actor<
	reacts_to<join_atom, std::string, scc>,
	replies_to<chat_atom, std::string>::with<std::string>,
	reacts_to<leave_atom, std::string, strong_actor_ptr>,
	//also reacts to down messages using the down handler
	replies_to<ls_atom>::with<std::vector<std::string>>
>;

// Definition of the state of the simple chat server. 
struct scs_state {
	std::vector<std::string> nicknames;
	std::vector<strong_actor_ptr> participants;
};

scs::behavior_type scs_impl(scs::stateful_pointer<scs_state> self) {
	// The actor should also monitor all chat participants and treat down messages in the same way leave messages are handled.
	self->set_down_handler([=](down_msg d) {
		// "Treat down messages in the same way leave messages are handled"
		// by converting them into leave messages and re-sending them!
		self->send(self, atom("leave"), to_string(d.reason), self->current_sender());
	});
	return {
		// "On a join message (that also contains a nickname and a handle to the new actor),
		// the sender of the message will be added to the list of chat participants."
		[=](join_atom, std::string name, strong_actor_ptr handle) {
			// add to nicknames
			self->state.nicknames.push_back(name);
			// monitor the new participant
			self->monitor(handle);
			// add to participants
			self->state.participants.push_back(handle);
		},
		// "On a chat message, the message is forwarded to all participants except for the sender of the chat message."
		[=](chat_atom, std::string message) -> std::string {
			auto sender = self->current_sender();
			for (strong_actor_ptr participant : self->state.participants) {
				if (participant != sender) {
					// Cast the strong_actor_ptr to type scc.
					// https://actor-framework.readthedocs.io/en/stable/ReferenceCounting.html#actor-cast
					scc peer = actor_cast<scc>(participant);
					// Forward the message to the participant.
					self->send(peer,message);
				};
			}
			},
			// On leave message, (that may contain a goodbye message), the sender of the leave message is erased from the list of chat participants.
			[=](leave_atom, std::string goodbyeMessage, strong_actor_ptr originalSender) {
				// Send the goodbye message
				self->send(self,atom("chat"),goodbyeMessage)
			},
			// On an ls message, the actor responds with a list of known nicknames to the sender.
			[=](ls_atom) -> std::vector<std::string> {

			}

	}
};

