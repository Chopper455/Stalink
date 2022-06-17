
#include "StaClientIpcProtocol.hpp"
#include "StaClientBase.hpp"

namespace stamask {

/**
 * Default constructor.
 * Sets channel to nullptr.
 */
StaClientIpcProtocol::StaClientIpcProtocol():
	mChannelPtr(nullptr),
	mCallbackPtr(nullptr) {}

/**
 * Deletes channel if it isn't null.
 */
StaClientIpcProtocol::~StaClientIpcProtocol() {
	if(mChannelPtr)
		delete mChannelPtr;
}

/**
 * Sets channel inside the protocol.
 * Accepts even the nullptr.
 * Protocol deletes previously set channel.
 */
void StaClientIpcProtocol::setChannel(
						IpcChannel* inChannelPtr) {
	if(mChannelPtr)
		delete mChannelPtr;

	mChannelPtr = inChannelPtr;
}

/**
 * Returns set up channel.
 * @return channel pointer
 */
IpcChannel* StaClientIpcProtocol::getChannel() {
	return mChannelPtr;
}


/**
 * Sets callback inside the protocol.
 * Accepts even the nullptr.
 * @param inCallbackPtr callback
 */
void StaClientIpcProtocol::setCallback(
						StaClientBase* inCallbackPtr) {
	mCallbackPtr = inCallbackPtr;
}

/**
 * Returns set up callback.
 * @return callback pointer
 */
StaClientBase* StaClientIpcProtocol::getCallback() {
	return mCallbackPtr;
}

/**
 * Returns empty string.
 * Error messages are passed in callback object.
 */
std::string StaClientIpcProtocol::getExecMessage() const {
	return "";
}


/**
 * Sends command to set hierarchy separator.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetHierarhySeparator& inCommand) {
	return executeWithSimpleResponse<CommandSetHierarhySeparator>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command for STA engine to exit.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandExit& inCommand) {
	return executeWithSimpleResponse<CommandExit>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to ping STA engine.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandPing& inCommand) {
	return executeWithSimpleResponse<CommandPing>(
			inCommand, mCallbackPtr, inCommand.mMsTimeout);
}

/**
 * Sends command to read liberty file.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadLibertyFile& inCommand) {
	return executeWithSimpleResponse<CommandReadLibertyFile>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to read liberty string.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadLibertyStream& inCommand) {
	return executeWithSimpleResponse<CommandReadLibertyStream>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to clear liberty libraries.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandClearLibs& inCommand) {
	return executeWithSimpleResponse<CommandClearLibs>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to read verilog file.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadVerilogFile& inCommand) {
	return executeWithSimpleResponse<CommandReadVerilogFile>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to read verilog string.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadVerilogStream& inCommand) {
	return executeWithSimpleResponse<CommandReadVerilogStream>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to link netlist with top block.
 * Receives back response with entities of timing graph.
 * Uses callback to set-up mapping for edges and vertexes.
 * See \link executeWithGraphResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandLinkTop& inCommand) {
	return executeWithSimpleResponse<CommandLinkTop>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to clear netlist and it's blocks.
 * Clears timing graph mappings in callback on success.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandClearNetlistBlocks& inCommand) {
	if(!executeWithSimpleResponse<CommandClearNetlistBlocks>(
			inCommand, mCallbackPtr))
		return false;

	mCallbackPtr->clearGraphMapping();
	return true;
}

/**
 * Sends command to create entire netlist.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandCreateNetlist& inCommand) {
	return executeWithSimpleResponse<CommandCreateNetlist>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to get graph data.
 * On receive writes out graph data.
 * Returns false on fail.
 */
bool StaClientIpcProtocol::execute(
		const CommandGetGraphData& inCommand,
		std::vector<VertexIdData>& outVertexIdToDataVec,
		std::vector<EdgeIdData>& outEdgeIdToDataVec) {

	ResponseGraphMap response;
	if(!sendReceiveCommand(inCommand, response))
		return false;

	if(!processResponseStatus(response, mCallbackPtr))
		return false;

	std::swap(outVertexIdToDataVec, response.mVertexIdToDataVec);
	std::swap(outEdgeIdToDataVec, response.mEdgeIdToDataVec);

	return true;
}

/**
 * Sends command to get timing data of graph vertexes.
 * On receive writes out timing data.
 * Returns false on fail.
 */
bool StaClientIpcProtocol::execute(
		const CommandGetGraphSlacksData& inCommand,
		std::vector<NodeTimingData>& outNodeTimingsVec) {
	ResponseGraphSlacks response;
	if(!sendReceiveCommand(inCommand, response))
		return false;

	if(!processResponseStatus(response, mCallbackPtr))
		return false;

	std::swap(outNodeTimingsVec, response.mNodeTimingsVec);

	return true;
}

/**
 * Sends command to set delays of several edges in a graph.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetArcsDelays& inCommand) {
	return executeWithSimpleResponse<CommandSetArcsDelays>(
			inCommand, mCallbackPtr);
}



/**
 * Sends command to connect net and instance/pin.
 * Client part must update graph data manually afterwards.
 * See \link executeWithGraphResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandConnectContextPinNet& inCommand) {
	return executeWithSimpleResponse<CommandConnectContextPinNet>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to disconnect net and instance/pin.
 * Client part must update graph data manually afterwards.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandDisconnectContextPinNet& inCommand) {
	return executeWithSimpleResponse<CommandDisconnectContextPinNet>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to read SPEF file.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadSpefFile& inCommand) {
	return executeWithSimpleResponse<CommandReadSpefFile>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to read SPEF string.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadSpefStream& inCommand) {
	return executeWithSimpleResponse<CommandReadSpefStream>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set summary capacitance of group of nets .
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetGroupNetCap& inCommand) {
	return executeWithSimpleResponse<CommandSetGroupNetCap>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to read SDF file.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadSdfFile& inCommand) {
	return executeWithSimpleResponse<CommandReadSdfFile>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to read SDF string.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandReadSdfStream& inCommand) {
	return executeWithSimpleResponse<CommandReadSdfStream>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to write SDF file.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandWriteSdfFile& inCommand) {
	return executeWithSimpleResponse<CommandWriteSdfFile>(
			inCommand, mCallbackPtr);
}




/**
 * Sends command to report timing paths.
 * Uses string in status response as holder of report.
 * Returns false if channel or callback is nullptr.
 * Returns false if response isn't OK.
 * @param inCommand command send
 * @param outReportStr string to write the report
 * @return execution status
 */
bool StaClientIpcProtocol::execute(
		const CommandReportTiming& inCommand,
		std::string& outReportStr) {
	if(!mChannelPtr || !mCallbackPtr)
		return false;

	ResponseCommExecStatus status;
	if(!sendReceiveCommand(inCommand, status))
		return false;

	if(!processResponseStatus(status, mCallbackPtr))
		return false;

	outReportStr = status.mStr;

	return true;
}

/**
 * Sends command to get main timing stats.
 * Returns false if channel or callback is nullptr.
 * Returns false if response isn't OK.
 * @param inCommand command send
 * @param outMinWNS worst slack for min condition
 * @param outMinWNS worst slack for max condition
 * @param outMinWNS total negative slack for min condition
 * @param outMinWNS total negative slack for max condition
 * @return execution status
 */
bool StaClientIpcProtocol::execute(
		const CommandGetDesignStats& inCommand,
		float& outMinWNS,
		float& outMaxWNS,
		float& outMinTNS,
		float& outMaxTNS) {
	if(!mChannelPtr || !mCallbackPtr)
		return false;

	ResponseDesignStats status;
	if(!sendReceiveCommand(inCommand, status))
		return false;

	if(!processResponseStatus(status, mCallbackPtr))
		return false;

	outMinWNS = status.mMinWslack;
	outMaxWNS = status.mMaxWslack;
	outMinTNS = status.mMinTNS;
	outMaxTNS = status.mMaxTNS;

	return true;
}


/**
 * Sends command to create clock.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandCreateClock& inCommand) {
	return executeWithSimpleResponse<CommandCreateClock>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to create generated clock.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandCreateGenClock& inCommand) {
	return executeWithSimpleResponse<CommandCreateGenClock>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set clock groups.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetClockGroups& inCommand) {
	return executeWithSimpleResponse<CommandSetClockGroups>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set clock latency.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetClockLatency& inCommand) {
	return executeWithSimpleResponse<CommandSetClockLatency>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set uncertainty between two clocks.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetInterClockUncertainty& inCommand) {
	return executeWithSimpleResponse<CommandSetInterClockUncertainty>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set uncertainty of single clock.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetSingleClockUncertainty& inCommand) {
	return executeWithSimpleResponse<CommandSetSingleClockUncertainty>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set uncertainty of single pin.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetSinglePinUncertainty& inCommand) {
	return executeWithSimpleResponse<CommandSetSinglePinUncertainty>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set port delay.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetPortDelay& inCommand) {
	return executeWithSimpleResponse<CommandSetPortDelay>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set input port transition.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetInPortTransition& inCommand) {
	return executeWithSimpleResponse<CommandSetInPortTransition>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set false paths.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetFalsePath& inCommand) {
	return executeWithSimpleResponse<CommandSetFalsePath>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set min/max delay.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetMinMaxDelay& inCommand) {
	return executeWithSimpleResponse<CommandSetMinMaxDelay>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set multicycle path.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetMulticyclePath& inCommand) {
	return executeWithSimpleResponse<CommandSetMulticyclePath>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to disable timing of single pin.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandDisableSinglePinTiming& inCommand) {
	return executeWithSimpleResponse<CommandDisableSinglePinTiming>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to disable inter-pin timing of an instance.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandDisableInstTiming& inCommand) {
	return executeWithSimpleResponse<CommandDisableInstTiming>(
			inCommand, mCallbackPtr);
}

/**
 * Sends command to set global timing derate.
 * See \link executeWithSimpleResponse
 */
bool StaClientIpcProtocol::execute(
		const CommandSetGlobalTimingDerate& inCommand) {
	return executeWithSimpleResponse<CommandSetGlobalTimingDerate>(
			inCommand, mCallbackPtr);
}


}


