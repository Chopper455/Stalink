#ifndef SRC_CHANNEL_YASMESSAGESERDES_HPP_
#define SRC_CHANNEL_YASMESSAGESERDES_HPP_


#include "AbsMessageSerdes.hpp"

#include "yas/mem_streams.hpp"
#include "yas/binary_iarchive.hpp"
#include "yas/binary_oarchive.hpp"
#include "yas/types/std/vector.hpp"
#include "yas/types/std/string.hpp"
#include "yas/types/std/map.hpp"
#include "yas/types/std/pair.hpp"

#include <iostream>


namespace stamask {


/**
 * Class that uses YAS serializer to target messages
 */
class YasMessageSerdes : public AbsMessageSerdes {

	/** buffer where to put serialized data */
    yas::shared_buffer mDataBuf;

public:

    /**
     * Default constructor.
     */
    YasMessageSerdes();

    /**
     * Empty destructor.
     */
	virtual ~YasMessageSerdes();

	/**
	 * Returns encoder ID.
	 * @return 41
	 */
	virtual uint32_t getEncoderId() const override;

public:

	virtual DataBlock serializeMessage(
			const Message& inMessage) override;

	virtual bool deserializeMessage(
			Message& inMessage,
			DataBlock inData) override;

protected:

	template <typename _MesgType>
	DataBlock serialize(
				const _MesgType& inMessage);

	template <typename _MesgType>
	bool deserialize(
			_MesgType& inMessage,
			DataBlock inData);

};


/**
 * Creates output stream and adds there data.
 * Returns pointer to the streams data buffer.
 * @param inMessage target message
 * @return data block
 */
template <typename _MesgType>
DataBlock YasMessageSerdes::serialize(
			const _MesgType& inMessage) {
	yas::mem_ostream oStream;
	yas::binary_oarchive<yas::mem_ostream, yas::binary | yas::no_header> oArch(oStream);
	oArch & inMessage;
	mDataBuf = oStream.get_shared_buffer();

	//std::cout << "Sent #bytes" << mDataBuf.size << std::endl;
	//print_bytes(mDataBuf.data.get(), mDataBuf.size);

	return {
		reinterpret_cast<const uint8_t *>(mDataBuf.data.get()),
		mDataBuf.size
	};
}


/**
 * Initializes stream with the data pointer and fills message data.
 * Does nothing and returns false if input data pointer is null.
 * @param inMessage target message
 * @param inData data block
 * @return success flag
 */
template <typename _MesgType>
bool YasMessageSerdes::deserialize(
		_MesgType& inMessage,
		DataBlock inData) {
	if(!inData.mDataPtr)
		return false;

	//std::cout << "Recieved #bytes" << inData.mBytesNum << std::endl;
	//print_bytes(inData.mDataPtr, inData.mBytesNum);

    yas::mem_istream iStream(
    		inData.mDataPtr,
			inData.mBytesNum);
    yas::binary_iarchive<yas::mem_istream, yas::binary | yas::no_header> iArch(iStream);
    iArch & inMessage;

    return true;
}




}


#endif /* SRC_CHANNEL_YASMESSAGESERDES_HPP_ */
