#include "StaClientBase.hpp"

#include <iostream>
#include <iterator>

namespace stamask {


/**
 * The constructor, makes protocol without any channel.
 */
StaClientBase::StaClientBase():
			IStaClient(),
			mProtocol(),
			mDivider('/'),
			mPathToPinUMap(),
			mSourcePinToVertexIdUMap(),
			mSinkPinToVertexIdUMap(),
			mPinPairToEdgeIdUMap(),
			mHasGraph(false),
			mNodeTimingDataVec(),
			mNodeMinCritFactorsVec(),
			mNodeMaxCritFactorsVec(),
			mHasGraphTiming(false) {
	mProtocol.setCallback(this);
}


/**
 * A destructor.
 */
StaClientBase::~StaClientBase() {}

/**
 * Sets channel inside the protocol.
 * Accepts even the nullptr.
 * Protocol deletes previously set channel.
 */
void StaClientBase::setChannel(
		IpcChannel* inChannelPtr) {
	mProtocol.setChannel(inChannelPtr);
}

/**
 * Returns channel that is preset in the protocol.
 * @return channel pointer
 */
IpcChannel* StaClientBase::getChannel() {
	return mProtocol.getChannel();
}

/**
 * Returns internal flag that graph data was set up.
 * @return flag that graph data was set up
 */
bool StaClientBase::hasGraph() const {
	return mHasGraph;
}


/**
 * Returns flag that graph&timing data were set up.
 * @return flag that graph&timing data were set up
 */
bool StaClientBase::hasTimings() const {
	return mHasGraph && mHasGraphTiming;
}

/**
 * Writes out calculated criticality of timing pin's node.
 * First tries locate node idx in one mapping, then another.
 * If edge sources has priority, then searches nodes in sources map.
 * Need this flag in case of bidirectional pins.
 * Writes out 0 and returns false if no graph or timing were loaded,
 * no timing node was found for pin or node idx is out of bounds.
 * Also returns false and outs 0 if pin pointer is null.
 * Writes out max-condition criticality if inMin flag is false.
 * @param inPinPtr target pin
 * @param outValue output criticalicty value
 * @param inMin flag to get min-condition criticality.
 * @param inEdgeSourcePriority flag to first search edge sources
 * @return success flag
 */
bool StaClientBase::getPinCriticality(
						const GenericPin* inPinPtr,
						float& outValue,
						bool inMin,
						bool inEdgeSourcePriority) const {
	if(!mHasGraph || !mHasGraphTiming) {
		//std::cout << "no graph and timing for criticality" << std::endl;
		outValue = 0;
		return false;
	}

	auto nodeIt = mSinkPinToVertexIdUMap.begin();
	bool hasData = false;

	//don't want to juggle pointers here
	if(inEdgeSourcePriority) {
		nodeIt = mSourcePinToVertexIdUMap.find(inPinPtr);
		hasData = nodeIt != mSourcePinToVertexIdUMap.end();
	} else {
		nodeIt = mSinkPinToVertexIdUMap.find(inPinPtr);
		hasData = nodeIt != mSinkPinToVertexIdUMap.end();
	}

	if(!hasData) {
		if(inEdgeSourcePriority) {
			nodeIt = mSinkPinToVertexIdUMap.find(inPinPtr);
			hasData = nodeIt != mSinkPinToVertexIdUMap.end();
		} else {
			nodeIt = mSourcePinToVertexIdUMap.find(inPinPtr);
			hasData = nodeIt != mSourcePinToVertexIdUMap.end();
		}
	}

	//out 0, return false if pin wasn't registered
	if(!hasData) {
		//std::cout << "pin wasn't registered" << std::endl;
		outValue = 0;
		return false;
	}

	//node index must be within bounds
	if((inMin && nodeIt->second >= mNodeMinCritFactorsVec.size()) ||
			(!inMin && nodeIt->second >= mNodeMaxCritFactorsVec.size())) {
		//std::cout << "node index must be within bounds" << std::endl;
		outValue = 0;
		return false;
	}

	if(inMin)
		outValue = mNodeMinCritFactorsVec[nodeIt->second];
	else
		outValue = mNodeMaxCritFactorsVec[nodeIt->second];

	return true;
}

bool StaClientBase::getPinTimingData(
		const GenericPin* inPinPtr,
		NodeTimingData& outValue) const {
		if(!mHasGraph || !mHasGraphTiming) {
		//std::cout << "no graph and timing for criticality" << std::endl;
		return false;
	}

	auto nodeIt = mSinkPinToVertexIdUMap.begin();
	bool hasData = false;

	//don't want to juggle pointers here
	nodeIt = mSourcePinToVertexIdUMap.find(inPinPtr);
	hasData = nodeIt != mSourcePinToVertexIdUMap.end();

	if(!hasData) {
		nodeIt = mSinkPinToVertexIdUMap.find(inPinPtr);
		hasData = nodeIt != mSinkPinToVertexIdUMap.end();
	}

	//out 0, return false if pin wasn't registered
	if(!hasData) {
		//std::cout << "pin wasn't registered" << std::endl;
		return false;
	}

	//node index must be within bounds
	if(nodeIt->second >= mNodeTimingDataVec.size()) {
		//std::cout << "node index must be within bounds" << std::endl;
		return false;
	}

	outValue = mNodeTimingDataVec[nodeIt->second];

	return true;
}




/**
 * Returns beginning iterator of name-pin map.
 * @return beginning name-pin iterator
 */
StaClientBase::StrToPinMap::const_iterator
StaClientBase::beginPathPins() const {
	//std::cout << "Path->pin mapping has " << mPathToPinUMap.size() << "entries" << std::endl;

	return mPathToPinUMap.begin();
}

/**
 * Returns ending iterator of name-pin map.
 * @return ending name-pin iterator
 */
StaClientBase::StrToPinMap::const_iterator
StaClientBase::endPathPins() const {
	return mPathToPinUMap.end();
}



/**
 * Sends command to set hierarchy separator in STA.
 * @param inSeparator separator
 */
void StaClientBase::setHierarchySeparator(
		char inSeparator) {
	CommandSetHierarhySeparator command;
	command.mStr = inSeparator;
	mDivider = inSeparator;

	mProtocol.execute(command);
}

/**
 * Sends command to exit STA.
 * Deletes the channel if command was successful.
 */
bool StaClientBase::exit() {
	CommandExit command;

	if(mProtocol.execute(command)) {
		//closing the connection channel
		//no need to delete channel, protocol does it on nullification
		mProtocol.setChannel(nullptr);


		return true;
	}

	return false;
}

/**
 * Sends command to check that STA server is alive.
 * Returns false if server returns fail status or timeout is reached.
 * @param inMsTimeout ms to wait
 */
bool StaClientBase::ping(unsigned long inMsTimeout) {
	CommandPing command;
	command.mMsTimeout = inMsTimeout;
	return mProtocol.execute(command);
}


/**
 * Sends command to read liberty file.
 * @param inFileName file name
 * @return success status
 */
bool StaClientBase::readLibertyFile(
						const std::string& inFileName) {
	CommandReadLibertyFile command;
	command.mStr = inFileName;

	return mProtocol.execute(command);
}
/**
 * Sends command to read liberty from stream.
 * @param inDataStream stream to read
 * @return success status
 */
bool StaClientBase::readLibertyStream(
		std::istream& inDataStream) {
	CommandReadLibertyStream command;
	command.mStr = std::string(
			std::istreambuf_iterator<char>(inDataStream), {});

	return mProtocol.execute(command);
}

/**
 * Sends command to clear out all stored liberty libraries from STA.
 * @return success status
 */
bool StaClientBase::clearLibraries()  {
	CommandClearLibs command;

	return mProtocol.execute(command);
}

/**
 * Sends command to read Verilog file.
 * @param inFileName file name
 * @return success status
 */
bool StaClientBase::readVerilogFile(
		const std::string& inFileName) {
	CommandReadVerilogFile command;
	command.mStr = inFileName;

	return mProtocol.execute(command);
}

/**
 * Sends command to read Verilog from stream.
 * @param inDataStream stream to read
 * @return success status
 */
bool StaClientBase::readVerilogStream(
		std::istream& inDataStream) {
	CommandReadVerilogStream command;
	command.mStr = std::string(
			std::istreambuf_iterator<char>(inDataStream), {});

	return mProtocol.execute(command);
}


/**
 * Method to build netlist structures from verilog data with given top-block.
 * All modules must be pre-read from Verilog files.
 * @param inTopBlockName top block name
 * @return success status
 */
bool StaClientBase::linkVerilogTopBlockNetlist(
						const std::string& inTopBlockName) {
	CommandLinkTop command;
	command.mStr = inTopBlockName;

	//have to reset status of graph and timing data
	clearGraphMapping();

	return mProtocol.execute(command);
}


/**
 * Method to build anew netlist structures with given top-block in STA.
 * Fills all data in a top-down manner and sends it to STA to create netlist.
 * Must do so because STA may have parent-tracking to handle instance contexts.
 * @param inBlockPtr top block
 * @return success status
 */
bool StaClientBase::linkCreateTopBlockNetlist(
						const GenericBlock* inBlockPtr) {
	if(!inBlockPtr)
		return false;

	//have to reset status of graph and timing data
	//because previous top-level was destroyed either way
	clearGraphMapping();

	CommandCreateNetlist command;
	BlockPtrToIdUMap blockToIdxUMap;
	if(!fillBlockData(inBlockPtr, command.mBlockDataVec, blockToIdxUMap, true))
		return false;

	return mProtocol.execute(command);
}

/**
 * Method to connect pin and net from top-block in STA.
 * @param inPinPtr pin pointer
 * @param inNetPtr net pointer
 * @return success status
 */
bool StaClientBase::connectTopContextPinNet(
						const GenericPin* inPinPtr,
						const GenericNet* inNetPtr) {
	if(!inPinPtr || !inNetPtr)
		return false;

	CommandConnectContextPinNet command;
	if(!fillPinNetCommand(command, {}, inPinPtr, inNetPtr))
		return false;

	return mProtocol.execute(command);
}

/**
 * Method to disconnect pin and net from top-block in STA.
 * @param inPinPtr pin pointer
 * @param inNetPtr net pointer
 * @return success status
 */
bool StaClientBase::disconnectTopContextPinNet(
						const GenericPin* inPinPtr,
						const GenericNet* inNetPtr) {
	if(!inPinPtr || !inNetPtr)
		return false;

	CommandDisconnectContextPinNet command;
	if(!fillPinNetCommand(command, {}, inPinPtr, inNetPtr))
		return false;

	return mProtocol.execute(command);
}

/**
 * Method to connect pin and net from inst-context in STA.
 * @param inInstContextVec top-bottom sequence of instances
 * @param inPinPtr pin pointer
 * @param inNetPtr net pointer
 * @return success status
 */
bool StaClientBase::connectContextPinNet(
						const std::vector<GenericInst*>& inInstContextVec,
						const GenericPin* inPinPtr,
						const GenericNet* inNetPtr) {
	if(!inPinPtr || !inNetPtr)
		return false;

	//graph data becomes invalid when netlist changes
	clearGraphMapping();

	CommandConnectContextPinNet command;
	if(!fillPinNetCommand(command, inInstContextVec, inPinPtr, inNetPtr))
		return false;

	return mProtocol.execute(command);
}

/**
 * Method to disconnect pin and net from inst-context in STA.
 * @param inInstContextVec top-bottom sequence of instances
 * @param inPinPtr pin pointer
 * @param inNetPtr net pointer
 * @return success status
 */
bool StaClientBase::disconnectContextPinNet(
						const std::vector<GenericInst*>& inInstContextVec,
						const GenericPin* inPinPtr,
						const GenericNet* inNetPtr)  {
	if(!inPinPtr || !inNetPtr)
		return false;

	//graph data becomes invalid when netlist changes
	clearGraphMapping();

	CommandDisconnectContextPinNet command;
	if(!fillPinNetCommand(command, inInstContextVec, inPinPtr, inNetPtr))
		return false;

	return mProtocol.execute(command);
}


/**
 * Sends command to clear current netlist from STA.
 * @return success status
 */
bool StaClientBase::clearNetlistBlocks() {
	CommandClearNetlistBlocks command;

	clearGraphMapping();
	return mProtocol.execute(command);
}


/**
 * Adds clock creation constraint.
 * Returns false if one of source has null pin pointer or creation failed.
 * @param inName clock name
 * @param inDescription clock comment
 * @param inPinPathsVec source pin paths
 * @param inAddFlag add flag
 * @param inPeriod clock period
 * @param inWaveformVec clock waveform
 * @return operation success
 */
bool StaClientBase::createClock(
		const std::string& inName,
		const std::string& inDescription,
		const std::vector<PinContextPath>& inPinPathsVec,
		bool inAddFlag,
		float inPeriod,
		const std::vector<float>& inWaveformVec) {
	CommandCreateClock command;
	command.mName = inName;
	command.mDescription = inDescription;
	command.mAddFlag = inAddFlag;
	command.mPeriod = inPeriod;
	command.mWaveformVec = inWaveformVec;

	if(!convertContextNameDataVec(
			inPinPathsVec, command.mPinPathsVec))
		return false;

	return mProtocol.execute(command);
}

/**
 * Adds generate clock creation constraint.
 * Allows null pin for master clock.
 * Returns false if one of source has null pin pointer or creation failed.
 * @param inName clock name
 * @param inDescription clock comment
 * @param inMasterClockPinPath path to master clock pin
 * @param inMasterClockName master clock name
 * @param inPinPathsVec source pin paths
 * @param inAddFlag add flag
 * @param inDivideFactor divide_by factor
 * @param inMultiplyFactor multiply_by factor
 * @param inDutyCycle duty cycle
 * @param inInvert inversion flag
 * @param inEdgesVec vector with edge timings
 * @param inEdgeShiftsVec vector with shifts in edge timings
 * @return operations success
 */
bool StaClientBase::createGeneratedClock(
		const std::string& inName,
		const std::string& inDescription,
		const PinContextPath& inMasterClockPinPath,
		const std::string& inMasterClockName,
		const std::vector<PinContextPath>& inPinPathsVec,
		bool inAddFlag,
	    int inDivideFactor,
		int inMultiplyFactor,
	    float inDutyCycle,
	    bool inInvert,
	    const std::vector<int>& inEdgesVec,
		const std::vector<float>& inEdgeShiftsVec) {
	CommandCreateGenClock command;
	command.mName = inName;
	command.mDescription = inDescription;
	command.mMasterClockName = inMasterClockName;
	command.mAddFlag = inAddFlag;
	command.mDivideFactor = inDivideFactor;
	command.mMultiplyFactor = inMultiplyFactor;
	command.mDutyCycle = inDutyCycle;
	command.mInvert = inInvert;
	command.mEdgesVec = inEdgesVec;
	command.mEdgeShiftsVec = inEdgeShiftsVec;

	if(inMasterClockPinPath.mObjectPtr)
		convertContextObjPath(
				inMasterClockPinPath, command.mMasterClockPinPath);
	if(!convertContextNameDataVec(
			inPinPathsVec, command.mPinPathsVec))
		return false;

	return mProtocol.execute(command);
}


/**
 * Creates constraint with clock groups.
 * @param inName name of groups constraint
 * @param inDescription constraint description
 * @param inLogicalExclusive flag of logic exclusivity
 * @param inPhysicalExclusive flag of physical exclusivity
 * @param inAsynchronous asynchronous groups flag
 * @param inAllowPaths allow paths flag
 * @param inClockGroupsVec vector of clock name groups
 * @return operations success
 */
bool StaClientBase::setClockGroups(
		const std::string& inName,
		const std::string& inDescription,
		bool inLogicalExclusive,
		bool inPhysicalExclusive,
		bool inAsynchronous,
		bool inAllowPaths,
		const std::vector<std::vector<std::string>>& inClockGroupsVec) {
	CommandSetClockGroups command;
	command.mName = inName;
	command.mDescription = inDescription;
	command.mLogicalExclusive = inLogicalExclusive;
	command.mPhysicalExclusive = inPhysicalExclusive;
	command.mAsynchronous = inAsynchronous;
	command.mAllowPaths = inAllowPaths;
	command.mClockGroupsVec = inClockGroupsVec;

	return mProtocol.execute(command);
}


/**
 * Adds constraint with clock source/network latency.
 * Allows null pin for clock.
 * @param inSource source latency flag
 * @param inMin min flag
 * @param inMax max flag
 * @param inEarly early (hold) timing flag
 * @param inLate late (setup) timing flag
 * @param inRise rise flag
 * @param inFall fall flag
 * @param inDelay latency value
 * @param inClockName target clock name
 * @param inPinPath target pin path
 * @return operation success
 */
bool StaClientBase::setClockLatency(
						bool inSource,
						bool inMin,
						bool inMax,
						bool inEarly,
						bool inLate,
						bool inRise,
						bool inFall,
						float inValue,
						const std::string& inClockName,
						const PinContextPath& inPinPath) {
	CommandSetClockLatency command;
	command.mSource = inSource;
	command.mMin = inMin;
	command.mMax = inMax;
	command.mEarly = inEarly;
	command.mLate = inLate;
	command.mRise = inRise;
	command.mFall = inFall;
	command.mValue = inValue;
	command.mClockName = inClockName;

	if(inPinPath.mObjectPtr)
		convertContextObjPath(
				inPinPath, command.mPinPath);

	return mProtocol.execute(command);
}


/**
 * Adds uncertainty constraint between two clocks.
 * @param inFromClockName name of from-clock
 * @param inFromRise rise of from-clock
 * @param inFromRise fall of from-clock
 * @param inToClockName name of to-clock
 * @param inToRise rise of to-clock
 * @param inToFall fall of to-clock
 * @param inSetup setup time flag
 * @param inHold hold time flag
 * @param inValue value to set
 * @return operations success
 */
bool StaClientBase::setClockToClockUncertainty(
						const std::string& inFromClockName,
						bool inFromRise,
						bool inFromFall,
						const std::string& inToClockName,
						bool inToRise,
						bool inToFall,
						bool inSetup,
						bool inHold,
						float inValue) {
	CommandSetInterClockUncertainty command;
	command.mFromClockName = inFromClockName;
	command.mFromRise = inFromRise;
	command.mFromFall = inFromFall;
	command.mToClockName = inToClockName;
	command.mToRise = inToRise;
	command.mToFall = inToFall;
	command.mSetup = inSetup;
	command.mHold = inHold;
	command.mValue = inValue;

	return mProtocol.execute(command);
}


/**
 * Adds uncertainty constraint of single clock.
 * @param inClockName clock name
 * @param inSetup setup time flag
 * @param inHold hold time flag
 * @param inValue value to set
 * @return operations success
 */
bool StaClientBase::setClockUncertainty(
						const std::string& inClockName,
						bool inSetup,
						bool inHold,
						float inValue) {
	CommandSetSingleClockUncertainty command;
	command.mClockName = inClockName;
	command.mSetup = inSetup;
	command.mHold = inHold;
	command.mValue = inValue;

	return mProtocol.execute(command);
}


/**
 * Adds clock uncertainty constraint of single pin.
 * Returns false if pin pointer is null.
 * @param inPinPath pin path
 * @param inSetup setup time flag
 * @param inHold hold time flag
 * @param inValue value to set
 * @return operations success
 */
bool StaClientBase::setPinUncertainty(
						const PinContextPath& inPinPath,
						bool inSetup,
						bool inHold,
						float inValue) {
	if(!inPinPath.mObjectPtr)
		return false;

	CommandSetSinglePinUncertainty command;
	command.mSetup = inSetup;
	command.mHold = inHold;
	command.mValue = inValue;

	convertContextObjPath(inPinPath, command.mPinPath);
	return mProtocol.execute(command);
}


/**
 * Adds delay of input/output port respective to clock.
 * Returns false if target pin pointer is null.
 * Ignores null pin pointer of the clock.
 * @param inIsInput input direction flag
 * @param inClockName name of reference clock
 * @param inClockPinPath name of source pin of ref clock (may be empty)
 * @param inClockFall flag of falling edge of clock
 * @param inLevelSensitive level sensivity
 * @param inDelayRise apply delay for rising edges
 * @param inDelayFall apply delay for falling edges
 * @param inDelayMax delay is max delay
 * @param inDelayMin delay is max delay
 * @param inAdd add to previous delays
 * @param inNetworkLatencyInc network delay is included in delay
 * @param inSourceLatencyInc source delay is included in delay
 * @param inDelay delay value
 * @param inTargetPortPin target port
 * @return operations success
 */
bool StaClientBase::setPortDelay(
						bool inIsInput,
						const std::string& inClockName,
						const PinContextPath& inClockPinPath,
						bool inClockFall,
						bool inLevelSensitive,
						bool inDelayRise,
						bool inDelayFall,
						bool inDelayMax,
						bool inDelayMin,
						bool inAdd,
						bool inNetworkLatencyInc,
						bool inSourceLatencyInc,
						float inDelay,
						const PinContextPath& inTargetPortPin) {
	if(!inTargetPortPin.mObjectPtr)
		return false;

	CommandSetPortDelay command;
	command.mIsInput = inIsInput;
	command.mClockName = inClockName;
	command.mClockFall = inClockFall;
	command.mLevelSensitive = inLevelSensitive;
	command.mDelayRise = inDelayRise;
	command.mDelayFall = inDelayFall;
	command.mDelayMax = inDelayMax;
	command.mDelayMin = inDelayMin;
	command.mAdd = inAdd;
	command.mNetworkLatencyInc = inNetworkLatencyInc;
	command.mSourceLatencyInc = inSourceLatencyInc;
	command.mDelay = inDelay;

	convertContextObjPath(
			inTargetPortPin, command.mTargetPortPin);
	convertContextObjPath(
			inClockPinPath, command.mClockPinPath);

	return mProtocol.execute(command);
}


/**
 * Adds edge transition time of input port's signal.
 * Returns false if target pin pointer is null.
 * Transition time is in seconds.
 * @param inDelayRise apply transition for rising edges
 * @param inDelayFall apply transition for falling edges
 * @param inDelayMax delay is max delay
 * @param inDelayMin delay is max delay
 * @param inValue transition time value
 * @param inTargetPortPin target port
 * @return operations success
 */
bool StaClientBase::setInputTransition(
						bool inDelayRise,
						bool inDelayFall,
						bool inDelayMax,
						bool inDelayMin,
						float inValue,
						const PinContextPath& inTargetPortPin) {
	if(!inTargetPortPin.mObjectPtr)
		return false;

	CommandSetInPortTransition command;
	command.mDelayRise = inDelayRise;
	command.mDelayFall = inDelayFall;
	command.mDelayMax = inDelayMax;
	command.mDelayMin = inDelayMin;
	command.mValue = inValue;

	convertContextObjPath(
			inTargetPortPin, command.mTargetPortPin);

	return mProtocol.execute(command);
}


/**
 * Adds paths to exclude from timing analysis.
 * @param inSetup apply for setup constraints
 * @param inHold apply for hold constraints
 * @param inComment constraint description
 * @param inFromRise rising edge flag of start points
 * @param inFromFall falling edge flag of start points
 * @param inFromPinPathsVec start point pin paths
 * @param inFromClocksVec start point clock names
 * @param inFromInstPathsVec start point inst paths
 * @param inThroughRise rising edge flag of middle points
 * @param inThroughFall falling edge flag of middle points
 * @param inThroughPinPathsVec middle point pin paths
 * @param inThroughInstPathsVec middle point inst paths
 * @param inThroughNetPathsVec start point net paths
 * @param inToRise rising edge flag of end points
 * @param inToFall falling edge flag of end points
 * @param inToPinPathsVec end point pin paths
 * @param inToClocksVec  end point clock names
 * @param inToInstPathsVec middle point inst paths
 * @return operation success
 */
bool StaClientBase::setFalsePath(
						bool inSetup,
						bool inHold,
						const std::string& inComment,
						bool inFromRise,
						bool inFromFall,
						const std::vector<PinContextPath>& inFromPinPathsVec,
						const std::vector<std::string>& inFromClocksVec,
						const std::vector<InstContextPath>& inFromInstPathsVec,
						bool inThroughRise,
						bool inThroughFall,
						const std::vector<PinContextPath>& inThroughPinPathsVec,
						const std::vector<InstContextPath>& inThroughInstPathsVec,
						const std::vector<NetContextPath>& inThroughNetPathsVec,
						bool inToRise,
						bool inToFall,
						const std::vector<PinContextPath>& inToPinPathsVec,
						const std::vector<std::string>& inToClocksVec,
						const std::vector<InstContextPath>& inToInstPathsVec) {
	CommandSetFalsePath command;
	command.mSetup = inSetup;
	command.mHold = inHold;
	command.mComment = inComment;

	fillPathMesgData(
			inFromRise, inFromFall, inFromPinPathsVec, inFromClocksVec, inFromInstPathsVec,
			inThroughRise, inThroughFall, inThroughPinPathsVec, inThroughInstPathsVec, inThroughNetPathsVec,
			inToRise, inToFall, inToPinPathsVec, inToClocksVec, inToInstPathsVec,
			command);

	return mProtocol.execute(command);
}

/**
 * Sets min/max delay constraint for specified paths.
 * @param inMinDelayFlag flag to add min_delay constraint
 * @param inValue delay value
 * @param inComment constraint description
 * @param inFromRise rising edge flag of start points
 * @param inFromFall falling edge flag of start points
 * @param inFromPinPathsVec start point pin paths
 * @param inFromClocksVec start point clock names
 * @param inFromInstPathsVec start point inst paths
 * @param inThroughRise rising edge flag of middle points
 * @param inThroughFall falling edge flag of middle points
 * @param inThroughPinPathsVec middle point pin paths
 * @param inThroughInstPathsVec middle point inst paths
 * @param inThroughNetPathsVec start point net paths
 * @param inToRise rising edge flag of end points
 * @param inToFall falling edge flag of end points
 * @param inToPinPathsVec end point pin paths
 * @param inToClocksVec  end point clock names
 * @param inToInstPathsVec middle point inst paths
 * @return operation success
 */
bool StaClientBase::setMinMaxDelay(
							bool inMinDelayFlag,
							float inValue,
							const std::string& inComment,
							bool inFromRise,
							bool inFromFall,
							const std::vector<PinContextPath>& inFromPinPathsVec,
							const std::vector<std::string>& inFromClocksVec,
							const std::vector<InstContextPath>& inFromInstPathsVec,
							bool inThroughRise,
							bool inThroughFall,
							const std::vector<PinContextPath>& inThroughPinPathsVec,
							const std::vector<InstContextPath>& inThroughInstPathsVec,
							const std::vector<NetContextPath>& inThroughNetPathsVec,
							bool inToRise,
							bool inToFall,
							const std::vector<PinContextPath>& inToPinPathsVec,
							const std::vector<std::string>& inToClocksVec,
							const std::vector<InstContextPath>& inToInstPathsVec) {
	CommandSetMinMaxDelay command;
	command.mMinDelayFlag = inMinDelayFlag;
	command.mValue = inValue;
	command.mComment = inComment;

	fillPathMesgData(
			inFromRise, inFromFall, inFromPinPathsVec, inFromClocksVec, inFromInstPathsVec,
			inThroughRise, inThroughFall, inThroughPinPathsVec, inThroughInstPathsVec, inThroughNetPathsVec,
			inToRise, inToFall, inToPinPathsVec, inToClocksVec, inToInstPathsVec,
			command);

	return mProtocol.execute(command);
}

/**
 * Sets multicycle clock constraint for specified paths.
 * @param inSetup apply for setup constraints
 * @param inHold apply for hold constraints
 * @param inStart apply multiplier to start clock
 * @param inEnd apply multiplier to end clock
 * @param inValue multiplier
 * @param inComment constraint description
 * @param inFromRise rising edge flag of start points
 * @param inFromFall falling edge flag of start points
 * @param inFromPinPathsVec start point pin paths
 * @param inFromClocksVec start point clock names
 * @param inFromInstPathsVec start point inst paths
 * @param inThroughRise rising edge flag of middle points
 * @param inThroughFall falling edge flag of middle points
 * @param inThroughPinPathsVec middle point pin paths
 * @param inThroughInstPathsVec middle point inst paths
 * @param inThroughNetPathsVec start point net paths
 * @param inToRise rising edge flag of end points
 * @param inToFall falling edge flag of end points
 * @param inToPinPathsVec end point pin paths
 * @param inToClocksVec  end point clock names
 * @param inToInstPathsVec middle point inst paths
 * @return operation success
 */
bool StaClientBase::setMulticyclePath(
						bool inSetup,
						bool inHold,
						bool inStart,
						bool inEnd,
						int inValue,
						const std::string& inComment,
						bool inFromRise,
						bool inFromFall,
						const std::vector<PinContextPath>& inFromPinPathsVec,
						const std::vector<std::string>& inFromClocksVec,
						const std::vector<InstContextPath>& inFromInstPathsVec,
						bool inThroughRise,
						bool inThroughFall,
						const std::vector<PinContextPath>& inThroughPinPathsVec,
						const std::vector<InstContextPath>& inThroughInstPathsVec,
						const std::vector<NetContextPath>& inThroughNetPathsVec,
						bool inToRise,
						bool inToFall,
						const std::vector<PinContextPath>& inToPinPathsVec,
						const std::vector<std::string>& inToClocksVec,
						const std::vector<InstContextPath>& inToInstPathsVec) {
	CommandSetMulticyclePath command;
	command.mSetup = inSetup;
	command.mHold = inHold;
	command.mStart = inStart;
	command.mEnd = inEnd;
	command.mValue = inValue;
	command.mComment = inComment;

	fillPathMesgData(
			inFromRise, inFromFall, inFromPinPathsVec, inFromClocksVec, inFromInstPathsVec,
			inThroughRise, inThroughFall, inThroughPinPathsVec, inThroughInstPathsVec, inThroughNetPathsVec,
			inToRise, inToFall, inToPinPathsVec, inToClocksVec, inToInstPathsVec,
			command);

	return mProtocol.execute(command);
}

/**
 * Adds constraint to disable timing arcs of portref.
 * Returns false if pin pointer is null.
 * @param inPinPath target pin path
 * @return operation success
 */
bool StaClientBase::setDisablePinTiming(
						const PinContextPath& inPinPath) {
	if(!inPinPath.mObjectPtr)
		return false;

	CommandDisableSinglePinTiming command;
	convertContextObjPath(inPinPath, command.mPinPath);

	return mProtocol.execute(command);
}

/**
 * Adds constraint to disable timing arcs of instance.
 * Returns false if one of pins is null.
 * @param inFromPinName path of from-pin
 * @param inToPinName path of to-pin
 * @return operation success
 */
bool StaClientBase::setDisableInstTiming(
						const std::vector<const GenericInst*>& mInstContextVec,
						const GenericPin* inFromPinPtr,
						const GenericPin* inToPinPtr) {
	if(!inFromPinPtr || !inToPinPtr)
		return false;

	CommandDisableInstTiming command;
	for(const GenericInst* instPtr : mInstContextVec)
		command.mInstContextVec.push_back(getName(instPtr));

	command.mFromPinName = getName(inFromPinPtr);
	command.mToPinName = getName(inToPinPtr);

	return mProtocol.execute(command);
}

/**
 * Adds global derate factor.
 * @param inCellDelay derate cell delays
 * @param inCellCheck derate cell check margins
 * @param inNetDelay derate interconnets
 * @param inData derate data paths
 * @param inClock derate clock paths
 * @param inEarly derate min delays
 * @param inLate derate max delays
 * @param inRise derate max transitions
 * @param inFall derate fall transitions
 * @param inValue derate factor
 * @return operations success
 */
bool StaClientBase::setGlobalTimingDerate(
						bool inCellDelay,
						bool inCellCheck,
						bool inNetDelay,
						bool inData,
						bool inClock,
						bool inEarly,
						bool inLate,
						bool inRise,
						bool inFall,
						float inValue) {
	CommandSetGlobalTimingDerate command;
	command.mCellDelay = inCellDelay;
	command.mCellCheck = inCellCheck;
	command.mNetDelay = inNetDelay;
	command.mData = inData;
	command.mClock = inClock;
	command.mEarly = inEarly;
	command.mLate = inLate;
	command.mRise = inRise;
	command.mFall = inFall;
	command.mValue = inValue;

	return mProtocol.execute(command);
}


/**
 * Sends command to read SPEF file.
 * @param inFileName file name
 * @return success status
 */
bool StaClientBase::readSPEFFile(
		const std::string& inFileName) {
	CommandReadSpefFile command;
	command.mStr = inFileName;

	return mProtocol.execute(command);
}

/**
 * Sends command to read SPEF stream.
 * @param inDataStream data stream
 * @return success status
 */
bool StaClientBase::readSPEFStream(
		std::istream& inDataStream) {
	CommandReadSpefStream command;
	command.mStr = std::string(
			std::istreambuf_iterator<char>(inDataStream), {});

	return mProtocol.execute(command);
}

/**
 * Sets lumped capacity for several nets in specific hierarchical context.
 * Capacitance value must be in farads.
 * @param inNetsDataVec nets and their context
 * @param inNetCapsVec capacitance values
 * @return success status
 */
bool StaClientBase::setNetsLumpCap(
		const std::vector<NetContextPath>& inNetsDataVec,
		const std::vector<float>& inNetCapsVec) {
	if(inNetsDataVec.empty())
		return true;

	CommandSetGroupNetCap command;

	//adding info of each net
	for(size_t i = 0; i < inNetsDataVec.size() && i < inNetCapsVec.size(); i++) {
		//skipping unexistent nets
		if(!inNetsDataVec[i].mObjectPtr)
			continue;

		command.mNetAddrsVec.push_back(ObjectContextNameData());
		command.mNetAddrsVec.back().mObjName = getName(inNetsDataVec[i].mObjectPtr);
		//adding names of instances in context
		for(const GenericInst* instPtr : inNetsDataVec[i].mInstContextVec)
			command.mNetAddrsVec.back().
					mInstContextVec.push_back(getName(instPtr));

		command.mValuesVec.push_back(inNetCapsVec[i]);
	}

	//making timing data invalid if wire load has changed
	clearTimingMapping();

	return mProtocol.execute(command);
}


/**
 * Sends command to read SDF file.
 * @param inFileName file name
 * @return success status
 */
bool StaClientBase::readSdfFile(
		const std::string& inFileName) {
	CommandReadSdfFile command;
	command.mStr = inFileName;

	return mProtocol.execute(command);
}

/**
 * Sends command to read SDF stream.
 * @param inDataStream data stream
 * @return success status
 */
bool StaClientBase::readSdfStream(
		std::istream& inDataStream) {
	CommandReadSdfStream command;
	command.mStr = std::string(
			std::istreambuf_iterator<char>(inDataStream), {});

	return mProtocol.execute(command);
}

/**
 * Sends command to write SDF file.
 * @param inFileName file name
 * @return success status
 */
bool StaClientBase::writeSdfFile(
		const std::string& inFileName) {
	CommandWriteSdfFile command;
	command.mStr = inFileName;

	return mProtocol.execute(command);
}

/**
 * Method to get mappings of nodes and edges of timing graph from STA engine.
 * First gets graph data, then maps netlist objects by their paths.
 * In the end matches netlist object to graph IDs.
 * Returns false if block pointer is null or fails to get data.
 * Clears all previously stored mappings through the callback mechanism.
 * @param inBlockPtr top block
 * @return success status
 */
bool StaClientBase::loadNetlistGraph(
		const GenericBlock* inBlockPtr) {
	if(!inBlockPtr)
		return false;

	//remapping the received graph data to pins with callback mechanism
	std::vector<VertexIdData> vertexIdToDataVec;
	std::vector<EdgeIdData> edgeIdToDataVec;
	CommandGetGraphData command;

	if(!mProtocol.execute(
			command, vertexIdToDataVec, edgeIdToDataVec))
		return false;

	clearGraphMapping();
	mHasGraph = addGraphMapping(inBlockPtr,
			vertexIdToDataVec, edgeIdToDataVec);
	return mHasGraph;
}

/**
 * Method to get pin slacks from STA engine.
 * Does nothing and returns false if timing graph wasn't loaded.
 * @return success status
 */
bool StaClientBase::loadNetlistSlacks() {
	if(!mHasGraph)
		return false;

	CommandGetGraphSlacksData command;
	mNodeTimingDataVec.clear();
	mHasGraphTiming = mProtocol.execute(command, mNodeTimingDataVec);

//	for(auto& data: mNodeTimingDataVec) {
//		std::cout << "Node " << data.mNodeId << ": "
//				<< " mHasTiming =" << data.mHasTiming
//				<< " mMaxWorstSlackAat =" << data.mMaxWorstSlackAat
//				<< " mMaxWorstSlackRat =" << data.mMaxWorstSlackRat
//				<< " mMinWorstSlackAat =" << data.mMinWorstSlackAat
//				<< " mMinWorstSlackRat =" << data.mMinWorstSlackRat
//				<< std::endl;
//	}


	//must be virtual to modify it in subclasses
	if(mHasGraphTiming) {
		mHasGraphTiming =
			calcNodeCritFactors(
				mNodeTimingDataVec, mNodeMinCritFactorsVec, true) &&
			calcNodeCritFactors(
				mNodeTimingDataVec, mNodeMaxCritFactorsVec, false);
		return mHasGraphTiming;
	}

	return false;
}

/**
 * Calculates timing criticalness factor on group's and node's data.
 * @param inData timing data of target node
 * @param inGroupMinSlack min slack of constraint group
 * @param inGroupMaxSlack max slack of constraint group
 * @param inGroupMaxRat max RAT of group's endpoint
 * @param inMinConstraint flag of min-constraint
 * @return criticality factor
 */
float StaClientBase::calcCritFactor(
						const NodeTimingData& inData,
						float inGroupMinSlack,
						float inGroupMaxSlack,
						float inGroupMaxRat,
						bool inMinConstraint) {

	float shiftAdd = inGroupMinSlack;

	//flipping the minimum slack to use it as a shifter of the paths group
	if(shiftAdd >= 0) {
		shiftAdd = 0;
	} else {
		shiftAdd *= -1;
	}

	//if max slack(with shift) exceeds divider, then use it instead
	inGroupMaxRat += shiftAdd;
	if(inGroupMaxSlack + shiftAdd > inGroupMaxRat)
		inGroupMaxRat = inGroupMaxSlack + shiftAdd;

	//zero, you shall not pass!
	if(inGroupMaxRat == 0)
		inGroupMaxRat += 1;

	//then finally calculating the node critical factor
	float nodeSlack = getNodeDataSlack(inData, inMinConstraint);
	return 1 - ( (nodeSlack + shiftAdd) / inGroupMaxRat );
}

float StaClientBase::getNodeDataSlack(
		const NodeTimingData& inData,
		bool inMinConstraint) {
	if(inMinConstraint)
		return inData.mMinWorstSlackAat - inData.mMinWorstSlackRat;

	return inData.mMaxWorstSlackRat - inData.mMaxWorstSlackAat;
}





/**
 * Calculates criticalness factors for all nodes based on their timing data.
 * Skips nodes that lack constrained path endpoints (i.e. unconstrained directly).
 * @param inNodeTimingData nodes timing data
 * @param outNodeCritFactorsVec nodes criticalness factors to set
 * @param inMinConstraint min-constraint flag
 * @return success flag
 */
bool StaClientBase::calcNodeCritFactors(
						const std::vector<NodeTimingData>& inNodeTimingData,
						std::vector<float>& outNodeCritFactorsVec,
						bool inMinConstraint) {
//	float undefEndpointMinSlack = std::numeric_limits<float>::max();
//	float undefEndpointMaxSlack = std::numeric_limits<float>::lowest();
//	float undefEndpointDivider = std::numeric_limits<float>::lowest();

	std::vector<float> endpointMinSlackVec;
	std::vector<float> endpointMaxSlackVec;
	std::vector<float> endpointDividerVec;

	float nodeSlack = 0;

	//process timing data of all nodes to find slack intervals and RAT dividers of endpoints

	for(const NodeTimingData& data : inNodeTimingData) {
		if(!data.mHasTiming)
			continue;

		nodeSlack = getNodeDataSlack(data, inMinConstraint);
//		//checking if it has endpoint, setting in appropriate place
//		if(data.mEndPointIdx >= inNodeTimingData.size()) {
//			if(nodeSlack > undefEndpointMaxSlack)
//				undefEndpointMaxSlack = nodeSlack;
//			if(nodeSlack < undefEndpointMinSlack)
//				undefEndpointMinSlack = nodeSlack;
//			if(inMinConstraint && data.mHasEndMinPathRat &&
//					data.mMinPathRat > undefEndpointDivider)
//				undefEndpointDivider = data.mMinPathRat;
//			if(!inMinConstraint && data.mHasEndMaxPathRat &&
//					data.mMaxPathRat > undefEndpointDivider)
//				undefEndpointDivider = data.mMaxPathRat;
//		}

		//NOTE: ignoring now unconstrained nodes and keeping their critical factor as 0
		if(data.mEndPointIdx >= inNodeTimingData.size())
			continue;

		//adding new entries in endpoint index is out of bounds
		if(data.mEndPointIdx >= endpointMinSlackVec.size()) {
			endpointMinSlackVec.resize(
					data.mEndPointIdx + 1,
					std::numeric_limits<float>::max());
			endpointMaxSlackVec.resize(
					data.mEndPointIdx + 1,
					std::numeric_limits<float>::lowest());
			endpointDividerVec.resize(
					data.mEndPointIdx + 1,
					std::numeric_limits<float>::lowest());
		}

		if(nodeSlack > endpointMaxSlackVec[data.mEndPointIdx])
			endpointMaxSlackVec[data.mEndPointIdx] = nodeSlack;
		if(nodeSlack < endpointMinSlackVec[data.mEndPointIdx])
			endpointMinSlackVec[data.mEndPointIdx] = nodeSlack;
		if(inMinConstraint && data.mHasEndMinPathRat &&
				data.mMinPathRat > endpointDividerVec[data.mEndPointIdx])
			endpointDividerVec[data.mEndPointIdx] = data.mMinPathRat;
		if(!inMinConstraint && data.mHasEndMaxPathRat &&
				data.mMaxPathRat > endpointDividerVec[data.mEndPointIdx])
			endpointDividerVec[data.mEndPointIdx] = data.mMaxPathRat;
	}


	//then calculating criticality of each node
	outNodeCritFactorsVec.clear();
	outNodeCritFactorsVec.resize(inNodeTimingData.size(), 0);


	for(size_t nIdx = 0; nIdx < inNodeTimingData.size(); nIdx++) {
		if(!inNodeTimingData[nIdx].mHasTiming)
			continue;

		//NOTE: ignoring now unconstrained nodes and keeping their critical factor as 0
		if(inNodeTimingData[nIdx].mEndPointIdx > inNodeTimingData.size())
			continue;

		//std::cout << "Calc crit factor of node #" << nIdx << std::endl;

		//calculating critical factor of the node
		outNodeCritFactorsVec[nIdx] = calcCritFactor(
				inNodeTimingData[nIdx],
				endpointMinSlackVec[inNodeTimingData[nIdx].mEndPointIdx],
				endpointMaxSlackVec[inNodeTimingData[nIdx].mEndPointIdx],
				endpointDividerVec[inNodeTimingData[nIdx].mEndPointIdx],
				inMinConstraint);
	}

	return true;
}

/**
 * Returns critical factor of node of the sink pin.
 * Does nothing and returns false if fails to find edge/node in the graph.
 * Does nothing and returns false if timing/graph data wasn't initialized.
 * Writes out zero if node index is out of bounds.
 * @param inSourcePinPtr beginning pin of the edge
 * @param inSinkPinPtr ending pin of the edge
 * @param inMinConstraint flag to get min-constraint factor
 * @param outCritFactor factor to set
 * @return success flag
 */
bool StaClientBase::getArcCritFactor(
						const GenericPin* inSourcePinPtr,
						const GenericPin* inSinkPinPtr,
						bool inMinConstraint,
						float& outCritFactor) {
	if(!mHasGraph || !mHasGraphTiming)
		return false;

	auto edgeIt = mPinPairToEdgeIdUMap.find(
			std::make_pair(inSourcePinPtr, inSinkPinPtr));
	if(edgeIt == mPinPairToEdgeIdUMap.end())
		return false;

	uint32_t nodeId = std::numeric_limits<uint32_t>::max();
	auto it = mSourcePinToVertexIdUMap.begin();

	//if it is intra-instance, then search sink first in sources, then in sinks
	if(getParentInstance(inSourcePinPtr) == getParentInstance(inSinkPinPtr)) {
		it = mSourcePinToVertexIdUMap.find(inSinkPinPtr);
		if(it != mSourcePinToVertexIdUMap.end()) {
			nodeId = it->second;
		} else {
			it = mSinkPinToVertexIdUMap.find(inSinkPinPtr);
			if(it != mSinkPinToVertexIdUMap.end())
				nodeId = it->second;
		}
	} else {
		//for inter-instance search in reverse order
		it = mSinkPinToVertexIdUMap.find(inSinkPinPtr);
		if(it != mSinkPinToVertexIdUMap.end()) {
			nodeId = it->second;
		} else {
			it = mSourcePinToVertexIdUMap.find(inSinkPinPtr);
			if(it != mSourcePinToVertexIdUMap.end())
				nodeId = it->second;
		}
	}

	//nothing to do if failed to find node ID
	if(nodeId >= mNodeMinCritFactorsVec.size() ||
			nodeId >= mNodeMaxCritFactorsVec.size())
		return false;

	if(inMinConstraint)
		outCritFactor = mNodeMinCritFactorsVec[nodeId];
	else
		outCritFactor = mNodeMaxCritFactorsVec[nodeId];

	return true;
}


/**
 * Sends command to set inter-pin delay in top-module context.
 * @param inSourcePinPtr source pin pointer
 * @param inSinkPinPtr sink pin pointer
 * @param inMin apply as min delay
 * @param inMax apply as max delay
 * @param inValue delay value
 */
bool StaClientBase::setInterPinArcDelay(
		const GenericPin* inSourcePinPtr,
		const GenericPin* inSinkPinPtr,
		bool inMin,
		bool inMax,
		float inValue) {
	if(!inSourcePinPtr || !inSinkPinPtr)
		return false;

	//making timing data invalid if arc delay changed
	clearTimingMapping();

	InterPinDelayData arcData = {inSourcePinPtr, inSinkPinPtr, inValue};
	return setInterPinArcDelays(
			{arcData},
			inMin, inMax);
}

/**
 * Method to set inter-pin delays in top-module context.
 * Ignores unexistent edges.
 * Returns false if non-empty input arcs vector becomes empty after filtering out.
 * Delay value is in seconds.
 * @param inArcDelaysVec pin pairs and their delays
 * @param inMin apply as min delay
 * @param inMax apply as max delay
 */
bool StaClientBase::setInterPinArcDelays(
						const std::vector<InterPinDelayData>& inArcDelaysVec,
						bool inMin,
						bool inMax) {
	//return true when there's nothing to do
	if(inArcDelaysVec.empty())
		return true;

	//nothing to do if graph mapping isn't ready
	if(!mHasGraph)
		return false;

	bool allOk = true;

	CommandSetArcsDelays command;
	command.mMin = inMin;
	command.mMax = inMax;

	std::string sourceName;
	std::string sinkName;

	//processing all arc to find vertexes
	//skipping pin pairs that have no edge ID
	for(const InterPinDelayData& arcData : inArcDelaysVec) {
		auto edgePairIt = mPinPairToEdgeIdUMap.equal_range(
				std::make_pair(arcData.mSourcePinPtr, arcData.mSinkPinPtr));

//		sourceName = getName(getParentInstance(arcData.mSourcePinPtr));
//		sourceName += "/";
//		sourceName += getName(arcData.mSourcePinPtr);
//		sinkName = getName(getParentInstance(arcData.mSinkPinPtr));
//		sinkName += "/";
//		sinkName += getName(arcData.mSinkPinPtr);

		if(edgePairIt.first == edgePairIt.second) {
//			std::cout << "no edge '" << sourceName << "' -> '" << sinkName << "'" << std::endl;
			//allOk = false;
			continue;
		}

		//setting delay for all edges between two nodes
		for(; edgePairIt.first != edgePairIt.second; edgePairIt.first++) {
//			std::cout << "edge '" << edgePairIt.first->second << ": " << sourceName << "' -> '" << sinkName << "'" << std::endl;

			command.mEdgeIdsVec.push_back(edgePairIt.first->second);
			command.mDelayValuesVec.push_back(arcData.mValue);
		}

	}

	if(!inArcDelaysVec.empty() && command.mEdgeIdsVec.empty())
		return false;

	//NOTE: sometimes STA engine may remove net edges on it own
	//so have to accept this fact and continue as it never happened
//	//there were unexistent edges
//	if(!allOk)
//		return false;

	//return true when there's nothing to do
	if(command.mDelayValuesVec.empty() ||
			command.mEdgeIdsVec.empty())
		return true;

	//making timing data invalid if arc delay changed
	clearTimingMapping();

	return mProtocol.execute(command);
}


/**
 * Sends command to report timing and gets resulting report.
 * @param inUnique report unique paths(without rise/fall variations)
 * @param inMin report min-constraint paths
 * @param inMax report max-constraint paths
 * @param inUnconstrained report unconstrained paths
 * @param inEndPointsNum amount of endpoints to report
 * @param inGroupsNum amount of groups to report
 * @param outReportStr report string to fill
 * @return success status
 */
bool StaClientBase::reportTiming(
		bool inUnique,
		bool inMin,
		bool inMax,
		bool inUnconstrained,
		uint32_t inEndPointsNum,
		uint32_t inGroupsNum,
		std::string& outReportStr) {
	CommandReportTiming command;

	command.mUniquePaths = inUnique;
	command.mMin = inMin;
	command.mMax = inMax;
	command.mUnconstrained = inUnconstrained;
	command.mEndPointsNum = inEndPointsNum;
	command.mGroupsNum = inGroupsNum;

	return mProtocol.execute(command, outReportStr);
}


/**
 * Clears all internal mappings of graph IDs and timings.
 */
void StaClientBase::clearGraphMapping() {
	//std::cout << "Clearing graph mapping" << std::endl;
	mPathToPinUMap.clear();
	mSourcePinToVertexIdUMap.clear();
	mSinkPinToVertexIdUMap.clear();
	mPinPairToEdgeIdUMap.clear();
	mHasGraph = false;

	clearTimingMapping();
}

/**
 * Clears all internal timing data.
 */
void StaClientBase::clearTimingMapping() {
	mNodeTimingDataVec.clear();
	mNodeMinCritFactorsVec.clear();
	mNodeMaxCritFactorsVec.clear();
	mHasGraphTiming = false;
}

/**
 * First remaps vertexes to pins
 * Then fills driver/sink pins -> vertexIds and pin pairs to edgeIDs.
 * Returns false if fails to fill data on each of steps.
 * @param inBlockPtr top-block
 * @param inVertexIdToDataVec vertexes data
 * @param inEdgeIdToDataVec edges data
 */
bool StaClientBase::addGraphMapping(
						const GenericBlock* inBlockPtr,
						const std::vector<VertexIdData>& inVertexIdToDataVec,
						const std::vector<EdgeIdData>& inEdgeIdToDataVec) {

	//preparing mapping of the netlist objects by their paths
	if(!addBlockPinsInNameMap(
			nullptr, inBlockPtr,
			mDivider, "", mPathToPinUMap))
		return false;

	//also have to map pins of top-level ports
	std::vector<GenericPort*> topPortsVec;
	getPorts(inBlockPtr, topPortsVec);
	for(GenericPort* portPtr : topPortsVec) {
		if(!portPtr || !getPortPin(portPtr))
			continue;

		mPathToPinUMap.emplace(
				getName(portPtr), getPortPin(portPtr));
	}


	//first matching vertexes and pins between each other
	std::vector<GenericPin*> vertexIdToPinVec;
	if(!matchVertexPins(inVertexIdToDataVec,
			mPathToPinUMap, vertexIdToPinVec))
		return false;

	//then fill pin -> vertexId mappings
	//depending on vertex "driver" flag
	if(!fillPinToVertexIdMaps(
			vertexIdToPinVec,
			inVertexIdToDataVec,
			mSourcePinToVertexIdUMap,
			mSinkPinToVertexIdUMap))
		return false;

	//finally filling the edge mapping
	if(!fillPinPairsToEdgeIdMap(
			vertexIdToPinVec,
			inEdgeIdToDataVec,
			mPinPairToEdgeIdUMap))
		return false;

	return true;
}

/**
 * For each vertex data creates a path.
 * Then by matching path to pin's one creates matching.
 * Returns false if fails to find one of the pins.
 * @param inVertexIdToDataVec data of vertexes
 * @param inPathToPinUMap paths of pins
 * @param outVertexIdToPinVec output vertex-pin mapping
 * @return operation success
 */
bool StaClientBase::matchVertexPins(
						const std::vector<VertexIdData>& inVertexIdToDataVec,
						const std::map<std::string, GenericPin*>& inPathToPinUMap,
						std::vector<GenericPin*>& outVertexIdToPinVec) {
	std::string vertexPath;
	auto pinIt = inPathToPinUMap.begin();

	outVertexIdToPinVec.clear();
	outVertexIdToPinVec.resize(
			inVertexIdToDataVec.size(),
			nullptr);

//	std::cout << "Matching vertex IDX and pins" << std::endl;

	//processing data of all vertexes
	//vertexId is ignores, because edge references the vector indexes now
	for(size_t dataIdx = 0; dataIdx < inVertexIdToDataVec.size(); dataIdx++) {
		//creating path
		vertexPath = "";
		for (const auto &instName : inVertexIdToDataVec[dataIdx].mContextInstNamesVec) {
			vertexPath += instName;
			vertexPath += mDivider;
		}
		vertexPath += inVertexIdToDataVec[dataIdx].mPinName;

		//searching the pin
		pinIt = inPathToPinUMap.find(vertexPath);
		if(pinIt == inPathToPinUMap.end()) {
//			std::cout << "vertex index " << dataIdx << " failed to find pin for: " << vertexPath << std::endl;
			return false;
		}

		if(!pinIt->second){
//			std::cout << "vertex index " << dataIdx << " failed to find pin for: " << vertexPath << std::endl;
			return false;
		}

//		std::cout << "vertex index " << dataIdx << " -> " << vertexPath << std::endl;

		outVertexIdToPinVec[dataIdx] = pinIt->second;
	}

	return true;
}

/**
 * Maps out vertex IDs of pins.
 * Writes out in driver or sink map depending on driver flag of vertex data.
 * @param inVertexIdToPinVec vertexId->pin mapping
 * @param inVertexIdToDataVec data of vertexes
 * @param inPathToPinUMap paths of pins
 * @param outVertexIdToPinVec output vertex-pin mapping
 * @return operation success
 */
bool StaClientBase::fillPinToVertexIdMaps(
						const std::vector<GenericPin*>& inVertexIdToPinVec,
						const std::vector<VertexIdData>& inVertexIdToDataVec,
						std::unordered_map<const GenericPin*, uint32_t>& mDriverPinToVertexIdUMap,
						std::unordered_map<const GenericPin*, uint32_t>& mSinkPinToVertexIdUMap) {
	GenericPin* pinPtr = nullptr;

	mDriverPinToVertexIdUMap.clear();
	mSinkPinToVertexIdUMap.clear();

	//processing data of all vertexes
	//pins are mapped by indexes in the vector
	for(size_t dataIdx = 0; dataIdx < inVertexIdToDataVec.size(); dataIdx++) {
		if(dataIdx >= inVertexIdToPinVec.size())
			return false;

		pinPtr = inVertexIdToPinVec[dataIdx];
		if(!pinPtr) {
			return false;
		}

//		std::string pinName;
//		if(getParentInstance(pinPtr)) {
//			pinName = getName(getParentInstance(pinPtr));
//			pinName += "/";
//		}
//		pinName += getName(pinPtr);

		//writing out mapping
		//mapping into vertex IDs from timing graph, not the vector indexes
		if(inVertexIdToDataVec[dataIdx].mIsDriver) {
//			std::cout << "source mapping vertex idx of pin '" << pinName
//					<< "' -> " << inVertexIdToDataVec[dataIdx].mVertexId << std::endl;
			mDriverPinToVertexIdUMap.emplace(pinPtr, inVertexIdToDataVec[dataIdx].mVertexId);
		} else {
//			std::cout << "sink mapping vertex idx of pin '" << pinName
//					<< "' -> " << inVertexIdToDataVec[dataIdx].mVertexId << std::endl;
			mSinkPinToVertexIdUMap.emplace(pinPtr, inVertexIdToDataVec[dataIdx].mVertexId);
		}
	}

	return true;
}

/**
 * Maps out pin pairs to edge IDs.
 * @param inVertexIdToPinVec vertexId->pin mapping
 * @param inVertexIdToDataVec data of edges
 * @param outPinPairToEdgeIdUMap output mapping
 * @return operation success
 */
bool StaClientBase::fillPinPairsToEdgeIdMap(
						const std::vector<GenericPin*>& inVertexIdToPinVec,
						const std::vector<EdgeIdData>& inEdgeIdToDataVec,
						PinsPairToEdgeIdUMMap& outPinPairToEdgeIdUMap) {
	const GenericPin* fromPinPtr = nullptr;
	const GenericPin* toPinPtr = nullptr;

	outPinPairToEdgeIdUMap.clear();

	//processing data of all edges
	for(const EdgeIdData& data : inEdgeIdToDataVec) {
		//have to find pins for both vertex ends
		if(data.mFromVertexId >= inVertexIdToPinVec.size() ||
				data.mToVertexId >= inVertexIdToPinVec.size()) {
//			std::cout << "out of bounds pins for edge " << data.mEdgeId <<
//					": " << data.mFromVertexId << " -> " << data.mToVertexId << std::endl;
			return false;
		}
		fromPinPtr = inVertexIdToPinVec[data.mFromVertexId];
		toPinPtr = inVertexIdToPinVec[data.mToVertexId];

//		std::string sourceName = getName(getParentInstance(fromPinPtr));
//		sourceName += "/";
//		sourceName += getName(fromPinPtr);
//		std::string sinkName = getName(getParentInstance(toPinPtr));
//		sinkName += "/";
//		sinkName += getName(toPinPtr);

		if(!fromPinPtr || !toPinPtr) {
//			std::cout << "failed to find pins for edge " << data.mEdgeId <<
//					": " << data.mFromVertexId << " -> " << data.mToVertexId  << std::endl;
			return false;
		}


//		std::cout << "registering edge " << data.mEdgeId <<
//				": '" << sourceName << "' -> '" << sinkName << "'" << std::endl;

		outPinPairToEdgeIdUMap.emplace(
				std::make_pair(fromPinPtr, toPinPtr),
				data.mEdgeId);
	}

	return true;
}

/**
 * Descends inside the block in DFS manner, maps pins of leaf insts by their paths.
 * For non-leaf masters accumulates inst names in path and descends deeper.
 * Returns false if block pointer is null.
 * @param inParentInstPtr previous parent instance
 * @param inBlockPtr target block
 * @param inDivider hierarchy divider to use
 * @param inPath current path
 * @param outPathToPinUMap output map to fill
 * @return operation success
 */
bool StaClientBase::addBlockPinsInNameMap(
						const GenericInst* inParentInstPtr,
						const GenericBlock* inBlockPtr,
						char inDivider,
						const std::string& inPath,
						std::map<std::string, GenericPin*>& outPathToPinMap) {
	if(!inBlockPtr)
		return false;

	//std::cout << "Filling pin paths mapping" << std::endl;

	bool allOk = true;

	std::string nextPath;
	std::vector<GenericInst*> childrenInstsVec;
	std::vector<GenericPin*> instPinsVec;
	GenericBlock* masterPtr = nullptr;

	//processing all children instances
	getBlockInsts(inParentInstPtr, inBlockPtr, childrenInstsVec);
	for(GenericInst* instPtr : childrenInstsVec) {
		if(!instPtr)
			continue;

		nextPath = inPath;
		nextPath += getName(instPtr);
		nextPath += inDivider;

		masterPtr = getMasterBlock(instPtr);
		if(!masterPtr)
			continue;

		//diving deeper if it isn't a leaf
		if(!isLeafBlock(masterPtr)) {
			allOk &= addBlockPinsInNameMap(
					instPtr, masterPtr, inDivider,
					nextPath, outPathToPinMap);
			continue;
		}

		//otherwise mapping all pins in the instance
		instPinsVec.clear();
		getInstPins(instPtr, instPinsVec);
		for(GenericPin* pinPtr : instPinsVec) {
			if(!pinPtr)
				continue;

			outPathToPinMap.emplace(
					nextPath+getName(pinPtr), pinPtr);
		}
	}

	return allOk;
}



/**
 * Sets all data in path message, doesn't check anything.
 */
bool StaClientBase::fillPathMesgData(
						bool inFromRise,
						bool inFromFall,
						const std::vector<PinContextPath>& inFromPinPathsVec,
						const std::vector<std::string>& inFromClocksVec,
						const std::vector<InstContextPath>& inFromInstPathsVec,
						bool inThroughRise,
						bool inThroughFall,
						const std::vector<PinContextPath>& inThroughPinPathsVec,
						const std::vector<InstContextPath>& inThroughInstPathsVec,
						const std::vector<NetContextPath>& inThroughNetPathsVec,
						bool inToRise,
						bool inToFall,
						const std::vector<PinContextPath>& inToPinPathsVec,
						const std::vector<std::string>& inToClocksVec,
						const std::vector<InstContextPath>& inToInstPathsVec,
						TimingPathMessageBase& outMessage) {
	outMessage.mFromRise = inFromRise;
	outMessage.mFromFall = inFromFall;
	convertContextNameDataVec(inFromPinPathsVec, outMessage.mFromPinPathsVec);
	outMessage.mFromClocksVec = inFromClocksVec;
	convertContextNameDataVec(inFromInstPathsVec, outMessage.mFromInstPathsVec);

	outMessage.mThroughRise = inThroughRise;
	outMessage.mThroughFall = inThroughFall;
	convertContextNameDataVec(inThroughPinPathsVec, outMessage.mThroughPinPathsVec);
	convertContextNameDataVec(inThroughInstPathsVec, outMessage.mThroughInstPathsVec);
	convertContextNameDataVec(inThroughNetPathsVec, outMessage.mThroughNetPathsVec);

	outMessage.mToRise = inToRise;
	outMessage.mToFall = inToFall;
	convertContextNameDataVec(inToPinPathsVec, outMessage.mToPinPathsVec);
	outMessage.mToClocksVec = inToClocksVec;
	convertContextNameDataVec(inToInstPathsVec, outMessage.mToInstPathsVec);

	return true;
}



}


