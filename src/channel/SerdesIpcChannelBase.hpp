#ifndef SRC_CHANNEL_SERDESIPCCHANNELBASE_HPP_
#define SRC_CHANNEL_SERDESIPCCHANNELBASE_HPP_

#include "IpcChannel.hpp"
#include "AbsMessageSerdes.hpp"

#include <string>
#include <iostream>

namespace stamask {



/**
 * Class uses serialization core to pack/unpack messages to send/receive them.
 * Connection must be implemented in subclasses.
 */
class SerdesIpcChannelBase : public IpcChannel {

	/** serdes to (un)pack messages */
	AbsMessageSerdes* mSerdesPtr;

public:

	SerdesIpcChannelBase(
			AbsMessageSerdes* inSerDesPtr);

	virtual ~SerdesIpcChannelBase();



	virtual uint32_t getEncoderId() const;

public:

	virtual EMessageStatus send(
			const Message& inMessage) override;

	virtual EMessageStatus popMessage(
			Message& outResponse) override;

protected:

	/**
	 * Sends packed message as a data block.
	 * @param inMesgType message type
	 * @param inBlock data block
	 * @return success flag
	 */
	virtual bool sendDataBlock(
			EMessageType inMesgType,
			DataBlock inBlock) = 0;

	/**
	 * Returns data block of received message.
	 * @param received data block
	 */
	virtual DataBlock getMessageDataBlock() = 0;


};






}







#endif /* SRC_CHANNEL_SERDESIPCCHANNELBASE_HPP_ */
