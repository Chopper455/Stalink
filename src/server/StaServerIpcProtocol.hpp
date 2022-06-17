#ifndef SRC_SERVER_STASERVERIPCPROTOCOL_HPP_
#define SRC_SERVER_STASERVERIPCPROTOCOL_HPP_


#include "channel/Messages.hpp"
#include "channel/IpcChannel.hpp"

#include "IStaServerHandler.hpp"


namespace stamask {


/**
 * Runs commands cycle, takes care of message interchange
 * with server interface when executing STA commands.
 */
class StaServerIpcProtocol {

	/** channel to interchange messages */
	IpcChannel* mChannelPtr;

	/** commands executor */
	IMessageExecutor* mStaHandlerPtr;

public:

	StaServerIpcProtocol(
			IpcChannel* inChannelPtr,
			IMessageExecutor* inStaHandlerPtr);

	~StaServerIpcProtocol();

	bool runCycle();

protected:

	bool sendStatusResponse(
			EMessageStatus inStatus,
			const std::string& inMessage);

	template <typename _MessageType>
	bool handleMessageWithStatus();

	bool handleReportTiming();

	bool handleGetDesignStats();

	bool handleGetGraphData();

	bool handleGetGraphSlacksData();

};


/**
 * Retrieves command data from channel, executes it and send status response.
 * Sends failed response if couldn't get message data.
 * Sends failed response if command execution returned false.
 * Returns false if fails on one of steps
 * @return success status
 */
template <typename _MessageType>
bool StaServerIpcProtocol::handleMessageWithStatus() {

	_MessageType command;
	EMessageStatus status = EMessageStatus::eMessageStatusOk;

	bool ok = true;
	if(mChannelPtr->popMessage(command) !=
			EMessageStatus::eMessageStatusOk) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	if(ok && !mStaHandlerPtr->execute(command)) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	ok &= sendStatusResponse(
			status, mStaHandlerPtr->getExecMessage());
	return ok;
}


}


#endif /* SRC_SERVER_STASERVERIPCPROTOCOL_HPP_ */
