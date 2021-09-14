
#include "SerdesIpcChannelBase.hpp"

#include <string>

namespace stamask {

/**
 * Constructor that sets serdes to use.
 * Doesn't throw anything if serdes pointer is null.
 * @param inSerDesPtr serdes pointer
 */
SerdesIpcChannelBase::SerdesIpcChannelBase(
		AbsMessageSerdes* inSerDesPtr):
				IpcChannel(),
				mSerdesPtr(inSerDesPtr) {}

/**
 * Deletes serdes if is was set.
 */
SerdesIpcChannelBase::~SerdesIpcChannelBase() {
	//cleaning up serdes
	if(mSerdesPtr)
		delete mSerdesPtr;
}


/**
 * Returns encoder ID of serdes.
 * If serdes is null, then returns -1 = max(uint32).
 * @return encoder ID
 */
uint32_t SerdesIpcChannelBase::getEncoderId() const {
	if(!mSerdesPtr)
		return -1;

	return mSerdesPtr->getEncoderId();
}

/**
 * Uses serdes to pack message and send the data block.
 * Returns failed status if serdes is null or send failed.
 * Otherwise returns OK status.
 * @param inMessage message to send
 * @return send status
 */
EMessageStatus SerdesIpcChannelBase::send(
						const Message& inMessage)  {
	if(!mSerdesPtr)
		return EMessageStatus::eMessageStatusFailed;

	if(!sendDataBlock(
			inMessage.getMesgType(),
			mSerdesPtr->serializeMessage(inMessage)))
		return EMessageStatus::eMessageStatusFailed;

	return EMessageStatus::eMessageStatusOk;
}

/**
 * Picks data block of received message and fills message with it.
 * Returns failed status if serdes is null or send failed.
 * Returns failed status if data block is null or failed to fill data.
 * Otherwise returns OK status.
 * @param outResponse message where to fill data
 * @return receive status
 */
EMessageStatus SerdesIpcChannelBase::popMessage(
						Message& outResponse) {
	if(!mSerdesPtr)
		return EMessageStatus::eMessageStatusFailed;

	DataBlock blk = getMessageDataBlock();
	if(!blk.mDataPtr)
		return EMessageStatus::eMessageStatusFailed;

	if(!mSerdesPtr->deserializeMessage(outResponse, blk))
		return EMessageStatus::eMessageStatusFailed;

	return EMessageStatus::eMessageStatusOk;
}



}


