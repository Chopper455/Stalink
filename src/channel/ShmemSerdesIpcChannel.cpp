
#include "ShmemSerdesIpcChannel.hpp"

#include <iostream>

#include <boost/thread/thread_time.hpp>


namespace stamask {


/**
 * Initializing constructor.
 * Sets serdes and prepares memory names to use.
 * Throws invalid_argument if serdes pointer is null.
 * @param inSerDesPtr serdes pointer
 * @param inMemName memory name
 * @param inServer server flag
 * @throw std::invalid_argument
 */
ShmemSerdesIpcChannel::ShmemSerdesIpcChannel(
		AbsMessageSerdes* inSerDesPtr,
		std::string inMemName,
		bool inServer):
				SerdesIpcChannelBase(inSerDesPtr),
				mMemBlockName(inMemName),
				mMemSegName(inMemName),
				mServer(inServer),
				mConnected(false),
				mShdMemBlock(),
				mShdMemBlockRegion(),
				mShdMemSeg(),
				mLock(),
				mShdCtrlPtr(nullptr),
				mShdMesgTypePtr(nullptr),
				mShdMesgSizePtr(nullptr),
				mShdEncodeTypePtr(nullptr),
				//mMesgStrPtr(nullptr),
				mShdMemBlockPtr(nullptr)
				{
	if(!inSerDesPtr)
		throw std::invalid_argument("serdes in channel constructor is null");

	mMemBlockName += ".blk";
	mMemBlockName += ".seg";
}

/**
 * Disconnects memory if it was connected.
 * Also deletes serdes object through superclass' destructor.
 */
ShmemSerdesIpcChannel::~ShmemSerdesIpcChannel() {
	disconnect();
}


/**
 * Connects to shared memory and set up class to interchange messages.
 * First disconnects if class is already connected.
 * In server mode checks if memory was already created, returns false then.
 * Opens shared memories, creates and initializes needed structures and pointers.
 * In the end checks that used encoder matches the one used in the class.
 * If encoder ID mismatches, then disconnects and throws runtime_error.
 * @return success flag
 * @throw std::runtime_error
 */
bool ShmemSerdesIpcChannel::connect() {
	//disconnect if it is already connected
	if(mConnected)
		disconnect();

	bool connected = false;

	//NOTE: it is safer to re-create shared mem than to reboot OS
//	//checking that another server is on
//	if(mServer && memoryExists())
//		return false;

	//opening the memory objects
	try {
		if(mServer) {
			//better to re-create memory if server previously terminated in unusual way
			bi::shared_memory_object::remove(mMemBlockName.c_str());
			bi::shared_memory_object::remove(mMemSegName.c_str());

			mShdMemBlock = bi::shared_memory_object(
					bi::open_or_create, mMemBlockName.c_str(), bi::read_write);
			mShdMemBlock.truncate(1024);

			mShdMemSeg = bi::managed_shared_memory(
					bi::open_or_create, mMemSegName.c_str(), 1024);
		} else {
			mShdMemBlock = bi::shared_memory_object(
					bi::open_only, mMemBlockName.c_str(), bi::read_write);

			mShdMemSeg = bi::managed_shared_memory(
					bi::open_only, mMemSegName.c_str());
		}
		connected = true;
	}
	catch (const std::exception &ex) {
		std::cout << "Exception: " << ex.what() << std::endl;
	}

	if(!connected)
		return false;

	//initializing all other members
	mShdCtrlPtr = mShdMemSeg.find_or_construct<ShmemMesgControl>("ctrl")();
	if(mServer)
		mShdCtrlPtr->mSenderFlag = mServer;

	mShdMesgTypePtr = mShdMemSeg.find_or_construct<EMessageType>("mesgType")(
			EMessageType::EMessageTypeNoMessage);
	mShdMesgSizePtr = mShdMemSeg.find_or_construct<uint32_t>("mesgSize")(0);
	mShdEncodeTypePtr = mShdMemSeg.find_or_construct<uint32_t>("encId")(getEncoderId());

	mShdMemBlockRegion = bi::mapped_region{mShdMemBlock, bi::read_write};
	mShdMemBlockPtr = static_cast<uint8_t*>(mShdMemBlockRegion.get_address());

	const boost::system_time timeout =
			boost::posix_time::microsec_clock::universal_time() +
			boost::posix_time::milliseconds(1000);

	mLock = bi::scoped_lock<bi::interprocess_mutex>(mShdCtrlPtr->mMutex, timeout);
	if(!mLock.owns()) {
		disconnect();
		throw std::runtime_error("connection channel was abandoned");
	}

	mLock.unlock();
	mConnected = true;

	//request name of serializer that is used on the other side
	//if it doesn't match, then throw an error and close connection
	if(getEncoderId() != *mShdEncodeTypePtr) {
		disconnect();
		throw std::runtime_error("serdes encoder ID doesn't match");
	}

	return true;
}

/**
 * Disconnects from the shared memory
 * Cla
 * Does nothing if class wasn't connected.
 *
 */
void ShmemSerdesIpcChannel::disconnect() {
	if(!mConnected)
		return;

	mShdCtrlPtr = nullptr;
	mShdMesgTypePtr = nullptr;
	mShdMesgSizePtr = nullptr;
	mShdEncodeTypePtr = nullptr;
	mShdMemBlockPtr = nullptr;

	mConnected = false;

	mLock = bi::scoped_lock<bi::interprocess_mutex>();
	mShdMemBlock = bi::shared_memory_object();
	mShdMemSeg = bi::managed_shared_memory();
	mShdMemBlockRegion = bi::mapped_region();

	if(!mServer)
		return;

	bi::shared_memory_object::remove(mMemBlockName.c_str());
	bi::shared_memory_object::remove(mMemSegName.c_str());
}


/**
 * Waits for arrival of message.
 * Checks that sender flag isn't equal to one owned.
 * Locks before and unlocks after the wait just in case for lock-safety.
 */
void ShmemSerdesIpcChannel::waitMessageArrival() {
	if(!mShdCtrlPtr)
		return;

    if(!mLock.owns())
    	mLock.lock();

	mShdCtrlPtr->mReadyCond.wait(
			mLock,
			[this] () {return this->senderMatches(); });

	mLock.unlock();
}

/**
 * Waits needed time period for arrival of message .
 * Checks that sender flag isn't equal to one owned.
 * Locks before and unlocks after the wait just in case for lock-safety.
 */
bool ShmemSerdesIpcChannel::waitTimeOutMessageArrival(
						unsigned long inMsTimeout) {
	if(!mShdCtrlPtr)
		return false;

	const boost::system_time timeout =
			boost::get_system_time() +
			boost::posix_time::milliseconds(inMsTimeout);

    if(!mLock.owns())
    	mLock.lock();

	mShdCtrlPtr->mReadyCond.timed_wait(
			mLock, timeout,
			[this] () {return this->senderMatches(); });

	mLock.unlock();
	return true;
}

/**
 * Returns type of current message type in shared memory.
 * Doesn't lock to read, is a little unsafe.
 * Returns no-message if pointer to message type is null.
 * @return message type
 */
EMessageType ShmemSerdesIpcChannel::peekMessageType() const {
	if(!mShdMesgTypePtr)
		return EMessageType::EMessageTypeNoMessage;

	return *mShdMesgTypePtr;
}

/**
 * Method to check that message was sent not by counterpart.
 * Is used as a predicate in waiting process.
 * Returns false if control structure is null.
 * @return flag to finish waiting
 */
bool ShmemSerdesIpcChannel::senderMatches() const {
	if(!mShdCtrlPtr)
		return false;

	return mShdCtrlPtr->mSenderFlag != mServer;
}

/**
 * Writes data block and message type in shared memory.
 * Also sets up sender flag and notifies one counterpart.
 * Locks-unlocks before-after data change.
 * Does nothing and returns false if some of involved data is absent.
 * Does nothing and returns false if data to send is nullptr.
 * @param inMesgType message type
 * @param inBlock data block to send
 * @return success flag
 */
bool ShmemSerdesIpcChannel::sendDataBlock(
						EMessageType inMesgType,
						DataBlock inBlock) {
	//checking that everything is in place
	if(!mConnected || !mShdCtrlPtr ||
			!mShdMesgTypePtr || !mShdMesgSizePtr)
		return false;

    if(!mLock.owns())
    	mLock.lock();

	*mShdMesgTypePtr = inMesgType;
	*mShdMesgSizePtr = inBlock.mBytesNum;

	//resizing shared block if message cannot fit inside
	boost::interprocess::offset_t blockSize = 0;
	mShdMemBlock.get_size(blockSize);

	if(inBlock.mBytesNum > blockSize) {
		mShdMemBlock.truncate(inBlock.mBytesNum);
		mShdMemBlockRegion = bi::mapped_region{mShdMemBlock, bi::read_write};
		mShdMemBlockPtr = static_cast<uint8_t*>(mShdMemBlockRegion.get_address());
		//setting flag for counterpart to update it's pointer to data block
		mShdCtrlPtr->mUpdatePtrFlag = true;

	}

	//copying if received bytes from the buffer
	//some serializers may provide nullptr block if message doesn't have any data
	if(inBlock.mDataPtr)
		memcpy((void*)mShdMemBlockPtr, (void*)inBlock.mDataPtr, inBlock.mBytesNum);

	//NOTE: this call here slows down the interchange HUMOGOUSLY!
	//DO NOT UNCOMMENT THIS!
	//mShdMemBlockRegion.flush(0, inBlock.mBitesNum, true);

	mShdCtrlPtr->mSenderFlag = mServer;

	mLock.unlock();
	mShdCtrlPtr->mReadyCond.notify_one();

	return true;
}

/**
 * Returns pointer to data and it's size.
 * Returns {nullptr, 0} if data is uninitialized or sender is current class.
 * @return data block
 */
DataBlock ShmemSerdesIpcChannel::getMessageDataBlock() {
	//must not return message block if client/server source flag doesn't match
	if(!mConnected ||
			!mShdCtrlPtr ||
			!mShdMemBlockPtr ||
			!mShdMesgSizePtr ||
			mShdCtrlPtr->mSenderFlag == mServer)
		return {nullptr, 0};

	if(mShdCtrlPtr->mUpdatePtrFlag) {
		mShdMemBlockRegion = bi::mapped_region{mShdMemBlock, bi::read_write};
		mShdMemBlockPtr = static_cast<uint8_t*>(mShdMemBlockRegion.get_address());
		mShdCtrlPtr->mUpdatePtrFlag = false;
	}

	return {mShdMemBlockPtr, *mShdMesgSizePtr};
}


/**
 * Checks that memory objects with set-up names exist.
 * @return existence of memory objects
 */
bool ShmemSerdesIpcChannel::memoryExists() {
	//a little mal-usage of try-catch here
	//but there's no other way to check
	try {
		bi::shared_memory_object testShObj(
				bi::open_only, mMemBlockName.c_str(), bi::read_only);
		bi::managed_shared_memory testShMem(
				bi::open_only, mMemSegName.c_str());

		return testShMem.check_sanity();
	} catch (...) {}

	return false;
}


}


