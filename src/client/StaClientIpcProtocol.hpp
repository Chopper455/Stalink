#ifndef SRC_NEW_STACLIENTIPCPROTOCOL_HPP_
#define SRC_NEW_STACLIENTIPCPROTOCOL_HPP_

#include "channel/Messages.hpp"
#include "channel/IpcChannel.hpp"
#include "common/IMessageExecutor.hpp"

#include <iostream>

namespace stamask {

class StaClientBase;

/**
 * Takes care of message interchange with client iface when executing STA commands.
 */
class StaClientIpcProtocol : public IMessageExecutor {

	/** channel to interchange messages */
	IpcChannel* mChannelPtr;

	/** callback to return massive data on execution */
	StaClientBase* mCallbackPtr;

public:

	StaClientIpcProtocol();

	virtual ~StaClientIpcProtocol();

	void setChannel(IpcChannel* inChannelPtr);

	IpcChannel* getChannel();


	void setCallback(StaClientBase* inCallbackPtr);

	StaClientBase* getCallback();

	virtual std::string getExecMessage() const;

	virtual bool execute(
			const CommandSetHierarhySeparator& inCommand);
	virtual bool execute(
			const CommandExit& inCommand);
	virtual bool execute(
			const CommandPing& inCommand);
	virtual bool execute(
			const CommandReadLibertyFile& inCommand);
	virtual bool execute(
			const CommandReadLibertyStream& inCommand);
	virtual bool execute(
			const CommandClearLibs& inCommand);
	virtual bool execute(
			const CommandReadVerilogFile& inCommand);
	virtual bool execute(
			const CommandReadVerilogStream& inCommand);
	virtual bool execute(
			const CommandLinkTop& inCommand);
	virtual bool execute(
			const CommandClearNetlistBlocks& inCommand);
	virtual bool execute(
			const CommandCreateNetlist& inCommand);
	virtual bool execute(
			const CommandGetGraphData& inCommand,
			std::vector<VertexIdData>& outVertexIdToDataVec,
			std::vector<EdgeIdData>& outEdgeIdToDataVec);
	virtual bool execute(
			const CommandGetGraphSlacksData& inCommand,
			std::vector<NodeTimingData>& outNodeTimingsVec);

	virtual bool execute(
			const CommandConnectContextPinNet& inCommand);
	virtual bool execute(
			const CommandDisconnectContextPinNet& inCommand);
	virtual bool execute(
			const CommandReadSpefFile& inCommand);
	virtual bool execute(
			const CommandReadSpefStream& inCommand);
	virtual bool execute(
			const CommandSetGroupNetCap& inCommand);
	virtual bool execute(
			const CommandReadSdfFile& inCommand);
	virtual bool execute(
			const CommandWriteSdfFile& inCommand);
	virtual bool execute(
			const CommandReadSdfStream& inCommand);
	virtual bool execute(
			const CommandSetArcsDelays& inCommand);
	virtual bool execute(
			const CommandReportTiming& inCommand,
			std::string& outReportStr);
	virtual bool execute(
			const CommandGetDesignStats& inCommand,
			float& outMinWNS,
			float& outMaxWNS,
			float& outMinTNS,
			float& outMaxTNS);

	virtual bool execute(
			const CommandCreateClock& inCommand);
	virtual bool execute(
			const CommandCreateGenClock& inCommand);
	virtual bool execute(
			const CommandSetClockGroups& inCommand);
	virtual bool execute(
			const CommandSetClockLatency& inCommand);
	virtual bool execute(
			const CommandSetInterClockUncertainty& inCommand);
	virtual bool execute(
			const CommandSetSingleClockUncertainty& inCommand);
	virtual bool execute(
			const CommandSetSinglePinUncertainty& inCommand);
	virtual bool execute(
			const CommandSetPortDelay& inCommand);
	virtual bool execute(
			const CommandSetInPortTransition& inCommand);
	virtual bool execute(
			const CommandSetFalsePath& inCommand);
	virtual bool execute(
			const CommandSetMinMaxDelay& inCommand);
	virtual bool execute(
			const CommandSetMulticyclePath& inCommand);
	virtual bool execute(
			const CommandDisableSinglePinTiming& inCommand);
	virtual bool execute(
			const CommandDisableInstTiming& inCommand);
	virtual bool execute(
			const CommandSetGlobalTimingDerate& inCommand);

protected:

	template<typename _CommandMessage>
	bool executeWithSimpleResponse(
			const _CommandMessage& inCommand,
			StaClientBase* inCallbackClientPtr,
			unsigned long inMsTimeout = 0);

	template<typename _CommandMessage, typename _ResponseMessage>
	bool sendReceiveCommand(
			const _CommandMessage& inMessage,
			_ResponseMessage& outResponse,
			unsigned long inMsTimeout = 0);


protected:

	template<typename _ResponseMessage>
	bool processResponseStatus(
			const _ResponseMessage& inResponse,
			StaClientBase* inCallbackClientPtr);


};


/**
 * Sends command, receives back simple response with execution status.
 * Returns false if response status isn't OK.
 * @param inCommand command to send
 * @param inCallbackClientPtr callback for printing
 * @return flag that response is OK
 */
template<typename _CommandMessage>
bool StaClientIpcProtocol::executeWithSimpleResponse(
		const _CommandMessage& inCommand,
		StaClientBase* inCallbackClientPtr,
		unsigned long inMsTimeout) {
	if(!mChannelPtr || !inCallbackClientPtr)
		return false;

	ResponseCommExecStatus status;
	if(!sendReceiveCommand(inCommand, status, inMsTimeout))
		return false;

	if(!processResponseStatus(status, inCallbackClientPtr))
		return false;

	return true;
}

/**
 * Sends message, writes out response itself if send failed.
 * Otherwise waits for response arrival.
 * If response isn't of expected message type, then writes out fail response.
 * Otherwise fills response message in channel and returns it's status.
 * If timeout isn't zero, then sets it as time to wait for response.
 * @param inMessage message to send
 * @param outResponse response data to get
 * @param inMsTimeout timeout of response wait period
 * @return flag of operation success
 */
template<typename _CommandMessage, typename _ResponseMessage>
bool StaClientIpcProtocol::sendReceiveCommand(
		const _CommandMessage& inMessage,
		_ResponseMessage& outResponse,
		unsigned long inMsTimeout) {
	if(!mChannelPtr)
		return false;

	EMessageStatus status = mChannelPtr->send(inMessage);
	if(status != EMessageStatus::eMessageStatusOk) {
		outResponse.mExecStatus = status;
		outResponse.mStr = "failed to send command";
		return false;
	}

	//here we are waiting for the response message
	if(inMsTimeout == 0) {
		mChannelPtr->waitMessageArrival();
	} else {
		if(!mChannelPtr->waitTimeOutMessageArrival(inMsTimeout)) {
			outResponse.mExecStatus = EMessageStatus::eMessageStatusTimeout;
			return false;
		}
	}

	bool isOk = true;
	//nothing to do if message type wasn't expected
	if(mChannelPtr->peekMessageType() != outResponse.getMesgType()) {
		outResponse.mExecStatus = EMessageStatus::eMessageStatusFailed;
		outResponse.mStr = "unexpected response type";
		return false;
	}

	//getting response data
	status = mChannelPtr->popMessage(outResponse);

	if(status != EMessageStatus::eMessageStatusOk) {
		outResponse.mExecStatus = EMessageStatus::eMessageStatusFailed;
		outResponse.mStr = "invalid response structure";
		isOk = false;
	}

	//mChannelPtr->popMessage();
	return isOk;
}


/**
 * Checks that message status is OK.
 * Uses callback to print an error otherwise, if callback isn't null.
 * If callback is null, then print option isn't called.
 * Returns true if response was OK.
 * @param inResponse response to check
 * @param inCallbackClientPtr callback for error messages
 * @return flag that response is OK
 */
template<typename _ResponseMessage>
bool StaClientIpcProtocol::processResponseStatus(
		const _ResponseMessage& inResponse,
		StaClientBase* inCallbackClientPtr) {
	if(inResponse.mExecStatus == EMessageStatus::eMessageStatusOk)
		return true;

	if(!inCallbackClientPtr)
		return false;

	switch(inResponse.mExecStatus) {
		case EMessageStatus::eMessageStatusFailed:
			inCallbackClientPtr->printError("command execution failed: " + inResponse.mStr);
			break;
		case EMessageStatus::eMessageStatusTimeout:
			inCallbackClientPtr->printError("command execution hit timeout: " + inResponse.mStr);
			break;
		case EMessageStatus::eMessageStatusUnsupported:
			inCallbackClientPtr->printError("command is unsupported: " + inResponse.mStr);
			break;
		default:
			break;
	}

	return false;
}









}


#endif /* SRC_NEW_STACLIENTIPCPROTOCOL_HPP_ */
