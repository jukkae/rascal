#ifndef MESSAGING_HPP
#define MESSAGING_HPP
struct Message;
struct Event;
namespace messaging {
	Message createMessageFromEvent(Event& event);
} // namespace message
#endif /* MESSAGING_HPP */
