#ifndef SRC_NEW_MESSAGES_HPP_
#define SRC_NEW_MESSAGES_HPP_

#include <cinttypes>
#include <string>
#include <vector>
#include <map>

namespace stamask {

/**
 * Enumeration with message types.
 */
enum EMessageType : uint16_t {

	EMessageTypeNoMessage,

	//------------------------
	//COMMANDS HERE
	//------------------------

	EMessageTypeExit,
	EMessageTypePing,
	EMessageTypeSetHierSeparator,

	EMessageTypeReadLibFile,
	EMessageTypeReadLibStream,
	EMessageTypeClearLibs,


	EMessageTypeReadVerilogFile,
	EMessageTypeReadVerilogStream,
	EMessageTypeLinkTop,
	EMessageTypeClearNetlistBlocks,

	EMessageTypeCreateNetlist,
	EMessageTypeGetGraphData,

	EMessageTypeConnectContextPinNet,
	EMessageTypeDisconnectContextPinNet,

	EMessageTypeReadSpefFile,
	EMessageTypeReadSpefStream,

	EMessageTypeSetGroupNetLumpCap,

	EMessageTypeReadSdfFile,
	EMessageTypeReadSdfStream,
	EMessageTypeWriteSdfFile,

	EMessageTypeGetGraphSlacksData,
	EMessageTypeSetArcsDelay,

	EMessageTypeCreateClock,
	EMessageTypeCreateGeneratedClock,
	EMessageTypeSetClockGroups,
	EMessageTypeSetClockLatency,
	EMessageTypeSetInterClockUncertainty,
	EMessageTypeSetSingleClockUncertainty,
	EMessageTypeSetSinglePinUncertainty,
	EMessageTypeSetSinglePortDelay,
	EMessageTypeSetInPortTransition,
	EMessageTypeSetFalsePath,
	EMessageTypeSetMinMaxDelay,
	EMessageTypeSetMulticyclePath,
	EMessageTypeDisableSinglePinTiming,
	EMessageTypeDisableInstTiming,
	EMessageTypeSetGlobalTimingDerate,

	EMessageTypeReportTiming,


	//------------------------
	//RESPONSES HERE
	//------------------------

	EMessageTypeExecutionStatus,
	EMessageTypeGraphMap,
	EMessageTypeGraphSlacks,
};



/**
 * Enumeration with possible statuses of command execution.
 */
enum EMessageStatus : uint16_t {
	eMessageStatusTimeout = 0,
	eMessageStatusOk = 1,
	eMessageStatusFailed = 2,
	eMessageStatusUnsupported = 3
};


//---------------------------------------------------------------
//commands to the STA

/**
 * Message to interchange commands and responses between client and server.
 * Need public data members to access them from serializers.
 */
class Message {
public:
	virtual ~Message() {}
	virtual EMessageType getMesgType() const = 0;
};


/**
 * Generic command with string data.
 */
class StringMessage : public Message {
public:
	std::string mStr;

public:
	StringMessage():
		mStr() {}
};


/**
 * Command for STA to exit.
 */
class CommandExit : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeExit;
	}
};

/**
 * Command to pion STA server.
 */
class CommandPing : public StringMessage {
public:
	unsigned long mMsTimeout = 0;
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypePing;
	}
};

/**
 * Command to set hierarchy separator.
 */
class CommandSetHierarhySeparator : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetHierSeparator;
	}
};

/**
 * Command to read liberty file.
 */
class CommandReadLibertyFile : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadLibFile;
	}
};


/**
 * Command to read liberty stream.
 */
class CommandReadLibertyStream : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadLibStream;
	}
};

/**
 * Command to clear liberty libraries.
 */
class CommandClearLibs : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeClearLibs;
	}
};


/**
 * Command to read Verilog file.
 */
class CommandReadVerilogFile : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadVerilogFile;
	}
};

/**
 * Command to read Verilog stream.
 */
class CommandReadVerilogStream : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadVerilogStream;
	}
};

/**
 * Command to link netlist top module.
 */
class CommandLinkTop : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeLinkTop;
	}
};

/**
 * Command to clear netlist blocks.
 */
class CommandClearNetlistBlocks : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeClearNetlistBlocks;
	}
};



/**
 * Info of a block's/instance's port/pin to transmit.
 */
struct PortData {
	std::string mName;
	bool mInput;
	bool mOutput;
	bool mBusFlag;
	uint32_t mRangeFrom;
	uint32_t mRangeTo;

	//vector with name indexes of connected nets
	std::vector<uint32_t> mConnNetIdxsVec;
};

/**
 * Info of a block's instance to transmit.
 */
struct InstanceData {
	std::string mName;
	uint32_t mMasterBlockIdx;
	std::vector<PortData> mPortDataVec;
};

/**
 * Info of a single block.
 * Has info of ports, instances and nets.
 */
struct BlockData {
	std::string mName;
	bool mTopFlag;
	bool mLeafFlag;

	std::vector<PortData> mPortDataVec;
	std::vector<InstanceData> mInstDataVec;

	std::vector<std::string> mNetNamesVec;
	std::string mGndNetName;
	std::string mVddNetName;
};


/**
 * Command to create and link netlist.
 */
class CommandCreateNetlist : public Message {
public:
	std::vector<BlockData> mBlockDataVec;

	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeCreateNetlist;
	}
};


/**
 * Command to connect net and pin in block.
 */
class CommandConnectContextPinNet : public Message {
public:
	//top-down instance context of parent block (empty for top-module)
	std::vector<std::string> mInstContextVec;

	std::string mNetName;
	std::string mInstName;
	std::string mPinName;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeConnectContextPinNet;
	}
};

/**
 * Command to disconnect net and pin in block.
 * Isn't a subtype of CommandConnectPinNet not to mix them in interfaces.
 */
class CommandDisconnectContextPinNet : public Message {
public:
	//top-down instance context of parent block (empty for top-module)
	std::vector<std::string> mInstContextVec;
	std::string mNetName;
	std::string mInstName;
	std::string mPinName;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeDisconnectContextPinNet;
	}
};





/**
 * Command to read SPEF file.
 */
class CommandReadSpefFile : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadSpefFile;
	}
};

/**
 * Command to read SPEF stream.
 */
class CommandReadSpefStream : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadSpefStream;
	}
};


/**
 * Structure to locate net by it's name in hierarchical context.
 */
struct ObjectContextNameData {
	std::vector<std::string> mInstContextVec;
	std::string mObjName;
};


/**
 * Command to set lump capacitance of group of nets.
 */
class CommandSetGroupNetCap : public Message {
public:
	std::vector<ObjectContextNameData> mNetAddrsVec;
	std::vector<float> mValuesVec;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetGroupNetLumpCap;
	}
};


/**
 * Command to read SDF file.
 */
class CommandReadSdfFile : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadSdfFile;
	}
};

/**
 * Command to write SDF file.
 */
class CommandWriteSdfFile : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeWriteSdfFile;
	}
};


/**
 * Command to read SDF stream.
 */
class CommandReadSdfStream : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReadSdfStream;
	}
};


/**
 * Command to set delays of several graph edges.
 */
class CommandSetArcsDelays : public Message {
public:
	std::vector<uint32_t> mEdgeIdsVec;
	std::vector<float> mDelayValuesVec; //caution! in seconds!
	bool mMin;
	bool mMax;
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetArcsDelay;
	}
};

/**
 * Command to get mapping of timing graph IDs to netlist paths.
 */
class CommandGetGraphData : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeGetGraphData;
	}
};

/**
 * Command to get slacks of all vertexes in timing graph.
 */
class CommandGetGraphSlacksData : public StringMessage {
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeGetGraphSlacksData;
	}
};


//==================================

/**
 * Command to create clock source.
 * Empty source pins bucket makes the clock virtual.
 */
class CommandCreateClock : public Message {
public:
	std::string mName;
	std::string mDescription;
	std::vector<ObjectContextNameData> mPinPathsVec;
	bool mAddFlag;
	float mPeriod;
	std::vector<float> mWaveformVec;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeCreateClock;
	}
};



/**
 * Command to create generated clock source.
 */
class CommandCreateGenClock : public Message {
public:
	std::string mName;
	std::string mDescription;
	ObjectContextNameData mMasterClockPinPath;
	std::string mMasterClockName;
	std::vector<ObjectContextNameData> mPinPathsVec;
	bool mAddFlag;
    int mDivideFactor;
	int mMultiplyFactor;
    float mDutyCycle;
    bool mInvert;
    std::vector<int> mEdgesVec;
	std::vector<float> mEdgeShiftsVec;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeCreateGeneratedClock;
	}
};




/**
 * Command to set relations between clock groups.
 */
class CommandSetClockGroups : public Message {
public:
	std::string mName;
	std::string mDescription;
	bool mLogicalExclusive;
	bool mPhysicalExclusive;
	bool mAsynchronous;
	bool mAllowPaths;
	std::vector<std::vector<std::string>> mClockGroupsVec;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetClockGroups;
	}
};



/**
 * Command to set clock latency.
 */
class CommandSetClockLatency : public Message {
public:
	bool mSource;
	bool mMin;
	bool mMax;
	bool mEarly;
	bool mLate;
	bool mRise;
	bool mFall;
	float mValue;
	std::string mClockName;
	ObjectContextNameData mPinPath;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetClockLatency;
	}
};


/**
 * Command to set clock uncertainty between two clocks.
 */
class CommandSetInterClockUncertainty : public Message {
public:
	std::string mFromClockName;
	bool mFromRise;
	bool mFromFall;
	std::string mToClockName;
	bool mToRise;
	bool mToFall;
	bool mSetup;
	bool mHold;
	float mValue;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetInterClockUncertainty;
	}
};


/**
 * Command to set clock uncertainty of single clock.
 */
class CommandSetSingleClockUncertainty : public Message {
public:
	std::string mClockName;
	bool mSetup;
	bool mHold;
	float mValue;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetSingleClockUncertainty;
	}
};


/**
 * Command to set clock uncertainty of single pin.
 */
class CommandSetSinglePinUncertainty : public Message {
public:
	ObjectContextNameData mPinPath;
	bool mSetup;
	bool mHold;
	float mValue;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetSinglePinUncertainty;
	}
};


/**
 * Command to set delay of single port.
 */
class CommandSetPortDelay : public Message {
public:
	bool mIsInput;
	std::string mClockName;
	ObjectContextNameData mClockPinPath;
	bool mClockFall;
	bool mLevelSensitive;
	bool mDelayRise;
	bool mDelayFall;
	bool mDelayMax;
	bool mDelayMin;
	bool mAdd;
	bool mNetworkLatencyInc;
	bool mSourceLatencyInc;
	float mDelay;
	ObjectContextNameData mTargetPortPin;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetSinglePortDelay;
	}
};


/**
 * Command to set delay of single port.
 */
class CommandSetInPortTransition : public Message {
public:
	bool mDelayRise;
	bool mDelayFall;
	bool mDelayMax;
	bool mDelayMin;
	float mValue;
	ObjectContextNameData mTargetPortPin;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetInPortTransition;
	}
};


/**
 * Basic message to transmit relations of timing paths.
 */
class TimingPathMessageBase : public Message {
public:
	bool mFromRise;
	bool mFromFall;
	std::vector<ObjectContextNameData> mFromPinPathsVec;
	std::vector<std::string> mFromClocksVec;
	std::vector<ObjectContextNameData> mFromInstPathsVec;

	bool mThroughRise;
	bool mThroughFall;
	std::vector<ObjectContextNameData> mThroughPinPathsVec;
	std::vector<ObjectContextNameData> mThroughInstPathsVec;
	std::vector<ObjectContextNameData> mThroughNetPathsVec;

	bool mToRise;
	bool mToFall;
	std::vector<ObjectContextNameData> mToPinPathsVec;
	std::vector<std::string> mToClocksVec;
	std::vector<ObjectContextNameData> mToInstPathsVec;
};


/**
 * Command to set false path.
 */
class CommandSetFalsePath : public TimingPathMessageBase {
public:
	bool mSetup;
	bool mHold;
	std::string mComment;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetFalsePath;
	}
};


/**
 * Command to set min/max delay of paths.
 */
class CommandSetMinMaxDelay : public TimingPathMessageBase {
public:
	bool mMinDelayFlag;
	float mValue;
	std::string mComment;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetMinMaxDelay;
	}
};

/**
 * Command to set multicycle paths.
 */
class CommandSetMulticyclePath : public TimingPathMessageBase {
public:
	bool mSetup;
	bool mHold;
	bool mStart;
	bool mEnd;
	int mValue;
	std::string mComment;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetMulticyclePath;
	}
};

/**
 * Command to disable timing of single pin.
 */
class CommandDisableSinglePinTiming : public Message {
public:
	ObjectContextNameData mPinPath;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeDisableSinglePinTiming;
	}
};

/**
 * Command to disable timing between two pins of an instance.
 */
class CommandDisableInstTiming : public Message {
public:
	std::vector<std::string> mInstContextVec;
	std::string mFromPinName;
	std::string mToPinName;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeDisableInstTiming;
	}
};



/**
 * Command to disable timing between two pins of an instance.
 */
class CommandSetGlobalTimingDerate : public Message {
public:
	bool mCellDelay;
	bool mCellCheck;
	bool mNetDelay;
	bool mData;
	bool mClock;
	bool mEarly;
	bool mLate;
	bool mRise;
	bool mFall;
	float mValue;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeSetGlobalTimingDerate;
	}
};

//========================================


/**
 * Command to report timing.
 */
class CommandReportTiming : public Message {
public:
	bool mUniquePaths = true;
	bool mMin = false;
	bool mMax = true;
	bool mUnconstrained = false;
	uint32_t mEndPointsNum = 1;
	uint32_t mGroupsNum = 1;
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeReportTiming;
	}
};


//---------------------------------------------------------------
//responses to commands execution



/**
 * Response with status of execution.
 * Also string can contain timing report as a successful response.
 */
class ResponseCommExecStatus : public StringMessage {
public:
	EMessageStatus mExecStatus =
			EMessageStatus::eMessageStatusUnsupported;
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeExecutionStatus;
	}
};



/**
 * Data of the vertex in timing graph.
 */
struct VertexIdData {
	std::vector<std::string> mContextInstNamesVec;
	std::string mPinName;
	bool mIsDriver;
	uint32_t mVertexId;
};

/**
 * Data of the edge in timing graph.
 */
struct EdgeIdData {
	uint32_t mFromVertexId;
	uint32_t mToVertexId;
	uint32_t mEdgeId;
};

/**
 * Response message with mapping of vertexes and edges of timing graph.
 */
class ResponseGraphMap : public ResponseCommExecStatus {
public:
	//index is also a vertex ID, if ID != index, then there's no such vertex
	std::vector<VertexIdData> mVertexIdToDataVec;

	//same indexing applies for the edges
	std::vector<EdgeIdData> mEdgeIdToDataVec;
public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeGraphMap;
	}
};



/**
 * Timing data of graph's node.
 * Edge timing may be derived from data of two vertices.
 */
struct NodeTimingData {
	uint32_t mNodeId = -1;
	bool mIsEndPoint = false;

	//flag to mark nodes that have no constraints
	bool mHasTiming = false;

	//for min-max slacks of both setup and hold analysis, rise/fall doesn't matter
	float mMinWorstSlackRat = -1e30;
	float mMinWorstSlackAat = 1e30;
	float mMaxWorstSlackRat = 1e30;
	float mMaxWorstSlackAat = -1e30;

	//flag to mark RAT of path endpoint, useful to calculate criticalness
	bool mHasEndMaxPathRat = false;
	float mMaxPathRat = 0;

	bool mHasEndMinPathRat = false;
	float mMinPathRat = 0;

	//need this to convert slacks into critical factors based on RATs of their own endpoints
	uint32_t mEndPointIdx = -1;
};


/**
 * Response message with timing of graph entities.
 */
class ResponseGraphSlacks : public ResponseCommExecStatus {
public:
	std::vector<NodeTimingData> mNodeTimingsVec;

public:
	virtual EMessageType getMesgType() const {
		return EMessageType::EMessageTypeGraphSlacks;
	}
};







}



#endif /* SRC_NEW_MESSAGES_HPP_ */
