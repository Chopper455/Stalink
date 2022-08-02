
#include "server/StaServerIpcProtocol.hpp"

#include <iostream>

namespace stamask {


/**
 * Initializes channel and commands handler.
 * Doesn't throw anything if nullptr is passed.
 * @param inChannelPtr channel to send/receive messages
 * @param inStaHandlerPtr commands executor
 */
StaServerIpcProtocol::StaServerIpcProtocol(
		IpcChannel* inChannelPtr,
		IMessageExecutor* inStaHandlerPtr):
			mChannelPtr(inChannelPtr),
			mStaHandlerPtr(inStaHandlerPtr) {}

/**
 * Deletes channel and executor if they aren't nullptr
 */
StaServerIpcProtocol::~StaServerIpcProtocol() {
	if(mChannelPtr)
		delete mChannelPtr;

	if(mStaHandlerPtr)
		delete mStaHandlerPtr;
}

/**
 * Runs cycle to wait for command and execute it.
 * Executes exit command and returns when exit command arrives.
 * Does nothing and returns false if channel or handler is nullptr.
 * If unsupported command arrives, then sends unsupported status response.
 * @return success flag
 */
bool StaServerIpcProtocol::runCycle() {
	if(!mChannelPtr || !mStaHandlerPtr)
		return false;

	bool toExit = false;
	bool handledCommand = false;

	while(!toExit) {
		handledCommand = true;
		mChannelPtr->waitMessageArrival();

		switch(mChannelPtr->peekMessageType()) {
			case EMessageType::EMessageTypeExit:
				handleMessageWithStatus<CommandExit>();
				toExit = true;
				break;
			case EMessageType::EMessageTypePing:
				handleMessageWithStatus<CommandPing>();
				break;
			case EMessageType::EMessageTypeSetHierSeparator:
				handleMessageWithStatus<CommandSetHierarhySeparator>();
				break;
			case EMessageType::EMessageTypeReadLibFile:
				handleMessageWithStatus<CommandReadLibertyFile>();
				break;
			case EMessageType::EMessageTypeReadLibStream:
				handleMessageWithStatus<CommandReadLibertyStream>();
				break;
			case EMessageType::EMessageTypeClearLibs:
				handleMessageWithStatus<CommandClearLibs>();
				break;
			case EMessageType::EMessageTypeReadVerilogFile:
				handleMessageWithStatus<CommandReadVerilogFile>();
				break;
			case EMessageType::EMessageTypeReadVerilogStream:
				handleMessageWithStatus<CommandReadVerilogStream>();
				break;
			case EMessageType::EMessageTypeLinkTop:
				handleMessageWithStatus<CommandLinkTop>();
				break;
			case EMessageType::EMessageTypeClearNetlistBlocks:
				handleMessageWithStatus<CommandClearNetlistBlocks>();
				break;
			case EMessageType::EMessageTypeCreateNetlist:
				handleMessageWithStatus<CommandCreateNetlist>();
				break;
			case EMessageType::EMessageTypeGetGraphData:
				handleGetGraphData();
				break;
			case EMessageType::EMessageTypeGetGraphSlacksData:
				handleGetGraphSlacksData();
				break;
			case EMessageType::EMessageTypeConnectContextPinNet:
				handleMessageWithStatus<CommandConnectContextPinNet>();
				break;
			case EMessageType::EMessageTypeDisconnectContextPinNet:
				handleMessageWithStatus<CommandDisconnectContextPinNet>();
				break;
			case EMessageType::EMessageTypeReadSpefFile:
				handleMessageWithStatus<CommandReadSpefFile>();
				break;
			case EMessageType::EMessageTypeReadSpefStream:
				handleMessageWithStatus<CommandReadSpefStream>();
				break;
			case EMessageType::EMessageTypeSetGroupNetLumpCap:
				handleMessageWithStatus<CommandSetGroupNetCap>();
				break;
			case EMessageType::EMessageTypeReadSdfFile:
				handleMessageWithStatus<CommandReadSdfFile>();
				break;
			case EMessageType::EMessageTypeWriteSdfFile:
				handleMessageWithStatus<CommandWriteSdfFile>();
				break;
			case EMessageType::EMessageTypeReadSdfStream:
				handleMessageWithStatus<CommandReadSdfStream>();
				break;
			case EMessageType::EMessageTypeSetArcsDelay:
				handleMessageWithStatus<CommandSetArcsDelays>();
				break;
			case EMessageType::EMessageTypeCreateClock:
				handleMessageWithStatus<CommandCreateClock>();
				break;
			case EMessageType::EMessageTypeCreateGeneratedClock:
				handleMessageWithStatus<CommandCreateGenClock>();
				break;
			case EMessageType::EMessageTypeSetClockGroups:
				handleMessageWithStatus<CommandSetClockGroups>();
				break;
			case EMessageType::EMessageTypeSetClockLatency:
				handleMessageWithStatus<CommandSetClockLatency>();
				break;
			case EMessageType::EMessageTypeSetInterClockUncertainty:
				handleMessageWithStatus<CommandSetInterClockUncertainty>();
				break;
			case EMessageType::EMessageTypeSetSingleClockUncertainty:
				handleMessageWithStatus<CommandSetSingleClockUncertainty>();
				break;
			case EMessageType::EMessageTypeSetSinglePinUncertainty:
				handleMessageWithStatus<CommandSetSinglePinUncertainty>();
				break;
			case EMessageType::EMessageTypeSetSinglePortDelay:
				handleMessageWithStatus<CommandSetPortDelay>();
				break;
			case EMessageType::EMessageTypeSetInPortTransition:
				handleMessageWithStatus<CommandSetInPortTransition>();
				break;
			case EMessageType::EMessageTypeSetPortPinLoad:
				handleMessageWithStatus<CommandSetPortPinLoad>();
				break;
			case EMessageType::EMessageTypeSetFalsePath:
				handleMessageWithStatus<CommandSetFalsePath>();
				break;
			case EMessageType::EMessageTypeSetMinMaxDelay:
				handleMessageWithStatus<CommandSetMinMaxDelay>();
				break;
			case EMessageType::EMessageTypeSetMulticyclePath:
				handleMessageWithStatus<CommandSetMulticyclePath>();
				break;
			case EMessageType::EMessageTypeDisableSinglePinTiming:
				handleMessageWithStatus<CommandDisableSinglePinTiming>();
				break;
			case EMessageType::EMessageTypeDisableInstTiming:
				handleMessageWithStatus<CommandDisableInstTiming>();
				break;
			case EMessageType::EMessageTypeSetGlobalTimingDerate:
				handleMessageWithStatus<CommandSetGlobalTimingDerate>();
				break;

			case EMessageType::EMessageTypeReportTiming:
				handleReportTiming();
				break;
			case EMessageType::EMessageTypeGetDesignStats:
				handleGetDesignStats();
				break;

			default:
				handledCommand = false;
		}

		if(handledCommand)
			continue;

		sendStatusResponse(
				EMessageStatus::eMessageStatusUnsupported,
				"unsupported command" + mChannelPtr->peekMessageType());
	}

	return true;
}


/**
 * Sends status response.
 * Returns true if message was successfully sent.
 * @param inStatus status to send
 * @param inMessage message to send
 * @return send success
 */
bool StaServerIpcProtocol::sendStatusResponse(
						EMessageStatus inStatus,
						const std::string& inMessage) {
	ResponseCommExecStatus response;
	response.mExecStatus = inStatus;
	response.mStr = inMessage;
	return mChannelPtr->send(response) == EMessageStatus::eMessageStatusOk;
}

/**
 * Handles command to report timing.
 * Operates like \link handleMessageWithStatus, but also sends out report string.
 * @return success status
 */
bool StaServerIpcProtocol::handleReportTiming() {

	CommandReportTiming command;
	EMessageStatus status = EMessageStatus::eMessageStatusOk;

	bool ok = true;
	if(mChannelPtr->popMessage(command) !=
			EMessageStatus::eMessageStatusOk) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	std::string reportStr;
	if(ok && !mStaHandlerPtr->execute(command, reportStr)) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	ResponseCommExecStatus response;
	response.mExecStatus = status;
	if(status == EMessageStatus::eMessageStatusOk)
		response.mStr = reportStr;

	return mChannelPtr->send(response) == EMessageStatus::eMessageStatusOk;
}

/**
 * Handles command to get design statistics.
 * Operates like \link handleMessageWithStatus, but also sends out report string.
 * @return success status
 */
bool StaServerIpcProtocol::handleGetDesignStats() {

	CommandGetDesignStats command;
	EMessageStatus status = EMessageStatus::eMessageStatusOk;

	bool ok = true;
	if(mChannelPtr->popMessage(command) !=
			EMessageStatus::eMessageStatusOk) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	std::string reportStr;
	float minWNS = 0;
	float maxWNS = 0;
	float minTNS = 0;
	float maxTNS = 0;

	if(ok && !mStaHandlerPtr->execute(command,
			minWNS, maxWNS, minTNS, maxTNS)) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	ResponseDesignStats response;
	response.mMinTNS = minTNS;
	response.mMaxTNS = maxTNS;
	response.mMinWslack = minWNS;
	response.mMaxWslack = maxWNS;

	response.mExecStatus = status;
	if(status == EMessageStatus::eMessageStatusOk)
		response.mStr = reportStr;

	return mChannelPtr->send(response) == EMessageStatus::eMessageStatusOk;
}


/**
 * Handles command to return mapping of objects in the timing graph.
 * Operates like \link handleMessageWithStatus, but sends out acquired data.
 * @return success status
 */
bool StaServerIpcProtocol::handleGetGraphData() {
	CommandGetGraphData command;
	EMessageStatus status = EMessageStatus::eMessageStatusOk;

	bool ok = true;
	if(mChannelPtr->popMessage(command) !=
			EMessageStatus::eMessageStatusOk) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	ResponseGraphMap response;
	if(ok && !mStaHandlerPtr->execute(
			command, response.mVertexIdToDataVec,
			response.mEdgeIdToDataVec)) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	response.mExecStatus = status;
	ok &= mChannelPtr->send(response) == EMessageStatus::eMessageStatusOk;
	return ok;
}


/**
 * Handles command to return slacks of vertexes in the timing graph.
 * Operates like \link handleMessageWithStatus, but sends out acquired data.
 * @return success status
 */
bool StaServerIpcProtocol::handleGetGraphSlacksData() {
	CommandGetGraphSlacksData command;
	EMessageStatus status = EMessageStatus::eMessageStatusOk;

	bool ok = true;
	if(mChannelPtr->popMessage(command) !=
			EMessageStatus::eMessageStatusOk) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	ResponseGraphSlacks response;
	if(ok && !mStaHandlerPtr->execute(
			command, response.mNodeTimingsVec)) {
		status = EMessageStatus::eMessageStatusFailed;
		ok = false;
	}

	response.mExecStatus = status;
	return ok && mChannelPtr->send(response) == EMessageStatus::eMessageStatusOk;
}



}


