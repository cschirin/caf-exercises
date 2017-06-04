// C++ Actor Framework Exercises
// Christian Schirin, 2017-06-03 
// Exercise 3: Simple Chat (Server)

#include "caf/all.hpp"

// "Implement an actor that receives join, chat, leave and ls messages.
// Pseudo Erlang notation: 
// {join, Name:string, Handle}
// {chat, Msg:string} -> Message: string
// {leave, Goodbye:string, Handle} 
// down_msg 
// {ls} -> NicknamesList: string

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
	reacts_to<leave_atom, std::string>,
	//also reacts to down messages using the down handler
	replies_to<ls_atom>::with<std::vector<std::string>>
>;

// Definition of the state of the simple chat server. 
struct scs_state {
	std::vector<std::string> nicknames;
	std::vector<scc> participants;
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
		[=](join_atom, std::string name, scc handle) {
		// add to nicknames
		// monitor the new participant
		// add to participants
		},
		// On a chat message, the message is forwarded to all participants except 
		// for the sender.
		[=](chat_atom, std::string name) -> std::string {

		},
		// On leave message, (that may contain a goodbye message), the sender of the leave message is erased from the list of chat participants.
		[=](leave_atom, std::string name) {

		}, 
		// On an ls message, the actor responds with a list of known nicknames to the sender.
		[=](ls_atom) -> std::vector<std::string> {

		}

	};


