#ifndef SRC_ABSMESSAGESERDES_HPP_
#define SRC_ABSMESSAGESERDES_HPP_


#include "Messages.hpp"

#include <cstdint>
#include <string>

#include <iostream>

namespace stamask {

/**
 * Message data.
 */
struct DataBlock {
	/** pointer to data block */
    const uint8_t* mDataPtr;
    /** amount of bytes in data block */
    uint64_t mBytesNum;
};


/**
 * Serdes interface.
 * Targets base message to decrease amount of methods.
 */
class AbsMessageSerdes {
public:
	/**
	 * Empty destructor.
	 */
	virtual ~AbsMessageSerdes() {}

	/**
	 * Returns ID of message encoder.
	 * @return econder ID
	 */
	virtual uint32_t getEncoderId() const = 0;

public:

	/**
	 * Serializes message and returns block with data pointer.
	 * @param inMessage target message
	 * @return data block
	 */
	virtual DataBlock serializeMessage(
			const Message& inMessage) = 0;

	/**
	 * Deserializes message from data block and sets data in message.
	 * @param outMessage message to fill
	 * @param inData data block
	 * @return operation success
	 */
	virtual bool deserializeMessage(
			Message& outMessage,
			DataBlock inData) = 0;
};


}


#endif /* SRC_ABSMESSAGESERDES_HPP_ */
