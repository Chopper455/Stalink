#ifndef SRC_COMMON_IMESSAGEEXECUTOR_HPP_
#define SRC_COMMON_IMESSAGEEXECUTOR_HPP_


#include "channel/Messages.hpp"


namespace stamask {


/**
 * Interface to handle commands on client or server side.
 */
class IMessageExecutor {

public:
	virtual ~IMessageExecutor() {}

	virtual std::string getExecMessage() const = 0;

	virtual bool execute(
			const CommandSetHierarhySeparator& inCommand) = 0;
	virtual bool execute(
			const CommandExit& inCommand) = 0;
	virtual bool execute(
			const CommandPing& inCommand) = 0;
	virtual bool execute(
			const CommandReadLibertyFile& inCommand) = 0;
	virtual bool execute(
			const CommandReadLibertyStream& inCommand) = 0;
	virtual bool execute(
			const CommandClearLibs& inCommand) = 0;
	virtual bool execute(
			const CommandReadVerilogFile& inCommand) = 0;
	virtual bool execute(
			const CommandReadVerilogStream& inCommand) = 0;
	virtual bool execute(
			const CommandLinkTop& inCommand) = 0;
	virtual bool execute(
			const CommandClearNetlistBlocks& inCommand) = 0;
	virtual bool execute(
			const CommandCreateNetlist& inCommand) = 0;
	virtual bool execute(
			const CommandGetGraphData& inCommand,
			std::vector<VertexIdData>& outVertexIdToDataVec,
			std::vector<EdgeIdData>& outEdgeIdToDataVec) = 0;
	virtual bool execute(
			const CommandGetGraphSlacksData& inCommand,
			std::vector<NodeTimingData>& outNodeTimingsVec) = 0;
	virtual bool execute(
			const CommandSetArcsDelays& inCommand) = 0;
	virtual bool execute(
			const CommandConnectContextPinNet& inCommand) = 0;
	virtual bool execute(
			const CommandDisconnectContextPinNet& inCommand) = 0;
	virtual bool execute(
			const CommandReadSpefFile& inCommand) = 0;
	virtual bool execute(
			const CommandReadSpefStream& inCommand) = 0;
	virtual bool execute(
			const CommandSetGroupNetCap& inCommand) = 0;
	virtual bool execute(
			const CommandReadSdfFile& inCommand) = 0;
	virtual bool execute(
			const CommandWriteSdfFile& inCommand) = 0;
	virtual bool execute(
			const CommandReadSdfStream& inCommand) = 0;
	virtual bool execute(
			const CommandCreateClock& inCommand) = 0;
	virtual bool execute(
			const CommandCreateGenClock& inCommand) = 0;
	virtual bool execute(
			const CommandSetClockGroups& inCommand) = 0;
	virtual bool execute(
			const CommandSetClockLatency& inCommand) = 0;
	virtual bool execute(
			const CommandSetInterClockUncertainty& inCommand) = 0;
	virtual bool execute(
			const CommandSetSingleClockUncertainty& inCommand) = 0;
	virtual bool execute(
			const CommandSetSinglePinUncertainty& inCommand) = 0;
	virtual bool execute(
			const CommandSetPortPinLoad& inCommand) = 0;
	virtual bool execute(
			const CommandSetPortDelay& inCommand) = 0;
	virtual bool execute(
			const CommandSetInPortTransition& inCommand) = 0;
	virtual bool execute(
			const CommandSetFalsePath& inCommand) = 0;
	virtual bool execute(
			const CommandSetMinMaxDelay& inCommand) = 0;
	virtual bool execute(
			const CommandSetMulticyclePath& inCommand) = 0;
	virtual bool execute(
			const CommandDisableSinglePinTiming& inCommand) = 0;
	virtual bool execute(
			const CommandDisableInstTiming& inCommand) = 0;
	virtual bool execute(
			const CommandSetGlobalTimingDerate& inCommand) = 0;
	virtual bool execute(
			const CommandReportTiming& inCommand,
			std::string& outReportStr) = 0;
	virtual bool execute(
			const CommandGetDesignStats& inCommand,
			float& outMinWNS,
			float& outMaxWNS,
			float& outMinTNS,
			float& outMaxTNS) = 0;

};

}



#endif /* SRC_COMMON_IMESSAGEEXECUTOR_HPP_ */
