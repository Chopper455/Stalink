#ifndef SRC_CHANNEL_SHMEMSERDESIPCCHANNEL_HPP_
#define SRC_CHANNEL_SHMEMSERDESIPCCHANNEL_HPP_


#include "SerdesIpcChannelBase.hpp"

#define BOOST_INTERPROCESS_ENABLE_TIMEOUT_WHEN_LOCKING
#define BOOST_INTERPROCESS_TIMEOUT_WHEN_LOCKING_DURATION_MS 500

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>



namespace bi = boost::interprocess;

namespace stamask {


/**
 * Structure to control data interchange.
 */
struct ShmemMesgControl {
    bi::interprocess_mutex mMutex;
    bi::interprocess_condition mReadyCond;
    volatile bool mSenderFlag;
    volatile bool mUpdatePtrFlag;
};


/**
 * Channel to transmit messages via shared memory.
 * Connection URL(memory name) is passed in constructor.
 */
class ShmemSerdesIpcChannel : public SerdesIpcChannelBase {

	/** name of memory block */
	std::string mMemBlockName;
	/** name of memory segment */
	std::string mMemSegName;

	/** server mode flag  */
	bool mServer;
	/** memory-connectedness flag */
	bool mConnected;


	/** shared memory block for message data */
	bi::shared_memory_object mShdMemBlock;

	/** shared memory region for control */
	bi::mapped_region mShdMemBlockRegion;

	/** shared memory segment for control */
	bi::managed_shared_memory mShdMemSeg;

	/** lock to control data access */
	bi::scoped_lock<bi::interprocess_mutex> mLock;


	/** memory control */
	mutable ShmemMesgControl* mShdCtrlPtr;

	/** message type to interchange */
	EMessageType* mShdMesgTypePtr;

	/** size of message block to interchange */
	uint32_t* mShdMesgSizePtr;

	/** encoder ID of server */
	uint32_t* mShdEncodeTypePtr;

	/** memory block to use */
	uint8_t* mShdMemBlockPtr;

public:

	ShmemSerdesIpcChannel(
			AbsMessageSerdes* inSerDesPtr,
			std::string inMemName,
			bool inServer);

	virtual ~ShmemSerdesIpcChannel();

	virtual bool connect() override;

	virtual void disconnect() override;


public:

	virtual void waitMessageArrival() override;

	virtual bool waitTimeOutMessageArrival(unsigned long inMsTimeout) override;

	virtual EMessageType peekMessageType() const override;

protected:

	bool senderMatches() const;

	virtual bool sendDataBlock(
			EMessageType inMesgType,
			DataBlock inBlock) override;

	virtual DataBlock getMessageDataBlock() override;

private:

	bool memoryExists();

};



}



#endif /* SRC_CHANNEL_SHMEMSERDESIPCCHANNEL_HPP_ */
