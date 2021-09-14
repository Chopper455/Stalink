#ifndef SRC_NEW_IPCCHANNEL_HPP_
#define SRC_NEW_IPCCHANNEL_HPP_

#include "Messages.hpp"

#include <string>

namespace stamask {


/**
 * Interface of class to interchange messages.
 * Connection URL is passed in subclass' constructor.
 */
class IpcChannel  {

public:

	/**
	 * Empty destructor.
	 */
	virtual ~IpcChannel() {}

	/**
	 * Connects channel to it's remote counterpart.
	 * @return success flag
	 */
	virtual bool connect() = 0;

	/**
	 * Dis channel from it's remote counterpart.
	 */
	virtual void disconnect() = 0;

public:

	/**
	 * Waits for arrival of any type of message.
	 */
	virtual void waitMessageArrival() = 0;

	/**
	 * Waits for arrival of any type of message for given amount of time.
	 * Returns false if time has run out.
	 * @param inMsTimeout milliseconds to wait
	 * @return flag that message arrived
	 */
	virtual bool waitTimeOutMessageArrival(unsigned long inMsTimeout) = 0;

	/**
	 * Returns type of message that has arrived.
	 * @return message type
	 */
	virtual EMessageType peekMessageType() const = 0;

public:

	/**
	 * Method to send a message.
	 * @param inMessage message to send
	 * @return success status
	 */
	virtual EMessageStatus send(
			const Message& inMessage) = 0;

	/**
	 * Method to get a message.
	 * @param outResponse message to fill
	 * @return success status
	 */
	virtual EMessageStatus popMessage(
			Message& outResponse) = 0;

};


}


#endif /* SRC_NEW_ICLIENTIPCCHANNEL_HPP_ */
