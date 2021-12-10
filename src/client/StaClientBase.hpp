#ifndef SRC_NEW_STACLIENTBASE_HPP_
#define SRC_NEW_STACLIENTBASE_HPP_


#include "IStaClient.hpp"
#include "StaClientIpcProtocol.hpp"
#include "AbsNetlistProcessorBase.hpp"

#include <boost/functional/hash.hpp>

#include <unordered_map>
#include <vector>
#include <limits>


namespace stamask {


/**
 * Base class to make client interface to STA via IPC connection.
 * Takes objects, converts data to send a command via IPC and acquire execution results.
 */
class StaClientBase:
		public IStaClient,
		public AbsNetlistProcessorBase {

	typedef std::pair<const GenericPin*, const GenericPin*>
			PinsPair;
	typedef std::unordered_multimap<
				PinsPair, uint32_t, boost::hash<PinsPair>>
			PinsPairToEdgeIdUMMap;
	typedef std::map<std::string, GenericPin*>
			StrToPinMap;

	/** protocol to interchange messages and get results */
	StaClientIpcProtocol mProtocol;

private:

	/** path divides to use */
	char mDivider;

	/** mapping from hierarchical path to pins (to match timing nodes) */
	std::map<std::string, GenericPin*> mPathToPinUMap;

	/** mapping from edge source to timing node index */
	std::unordered_map<const GenericPin*, uint32_t> mSourcePinToVertexIdUMap;

	/** mapping from edge target to timing node index */
	std::unordered_map<const GenericPin*, uint32_t> mSinkPinToVertexIdUMap;

	/** mapping from (source, target) pair of pins to ID of timing edge  */
	PinsPairToEdgeIdUMMap mPinPairToEdgeIdUMap;

	/** flag that timing graph was loaded */
	bool mHasGraph;

private:

	/** timing data of graph nodes */
	std::vector<NodeTimingData> mNodeTimingDataVec;

	/** calculated min-constraint criticality of graph nodes */
	std::vector<float> mNodeMinCritFactorsVec;

	/** calculated max-constraint criticality of graph nodes */
	std::vector<float> mNodeMaxCritFactorsVec;

	/** flag that timing data was loaded */
	bool mHasGraphTiming;


public:

	StaClientBase();

	virtual ~StaClientBase();

	void setChannel(IpcChannel* inChannelPtr);

	IpcChannel* getChannel();

public:

	bool hasGraph() const;

	bool hasTimings() const;

	bool getPinCriticality(
			const GenericPin* inPinPtr,
			float& outValue,
			bool inMin,
			bool inEdgeSourcePriority = true) const;
			
	bool getPinTimingData(
			const GenericPin* inPinPtr,
			NodeTimingData& outValue) const;

	StrToPinMap::const_iterator beginPathPins() const;
	StrToPinMap::const_iterator endPathPins() const;


			
public:

	virtual bool exit();

	virtual bool ping(
			unsigned long inMsTimeout);

	virtual void setHierarchySeparator(
			char inSeparator);

	virtual bool readLibertyFile(
			const std::string& inFileName);

	virtual bool readLibertyStream(
			std::istream& inDataStream);

	virtual bool clearLibraries();


	virtual bool readVerilogFile(
			const std::string& inFileName);

	virtual bool readVerilogStream(
			std::istream& inDataStream);

	virtual bool linkVerilogTopBlockNetlist(
			const std::string& inTopBlockName);


	virtual bool linkCreateTopBlockNetlist(
			const GenericBlock* inBlockPtr);

	virtual bool connectTopContextPinNet(
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr);

	virtual bool disconnectTopContextPinNet(
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr);

	virtual bool connectContextPinNet(
			const std::vector<GenericInst*>& inInstContextVec,
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr);

	virtual bool disconnectContextPinNet(
			const std::vector<GenericInst*>& inInstContextVec,
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr);

	virtual bool clearNetlistBlocks();

public:

	virtual bool createClock(
			const std::string& inName,
			const std::string& inDescription,
			const std::vector<PinContextPath>& inPinPathsVec,
			bool inAddFlag,
			float inPeriod,
			const std::vector<float>& inWaveformVec);

	virtual bool createGeneratedClock(
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
			const std::vector<float>& inEdgeShiftsVec);

	virtual bool setClockGroups(
			const std::string& inName,
			const std::string& inDescription,
			bool inLogicalExclusive,
			bool inPhysicalExclusive,
			bool inAsynchronous,
			bool inAllowPaths,
			const std::vector<std::vector<std::string>>& inClockGroupsVec);

	virtual bool setClockLatency(
			bool inSource,
			bool inMin,
			bool inMax,
			bool inEarly,
			bool inLate,
			bool inRise,
			bool inFall,
			float inValue,
			const std::string& inClockName,
			const PinContextPath& inPinPath);

	virtual bool setClockToClockUncertainty(
			const std::string& inFromClockName,
			bool inFromRise,
			bool inFromFall,
			const std::string& inToClockName,
			bool inToRise,
			bool inToFall,
			bool inSetup,
			bool inHold,
			float inValue);

	virtual bool setClockUncertainty(
			const std::string& inClockName,
			bool inSetup,
			bool inHold,
			float inValue);

	virtual bool setPinUncertainty(
			const PinContextPath& inPinPath,
			bool inSetup,
			bool inHold,
			float inValue);

	virtual bool setPortDelay(
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
			const PinContextPath& inTargetPortPin);

	virtual bool setInputTransition(
			bool inDelayRise,
			bool inDelayFall,
			bool inDelayMax,
			bool inDelayMin,
			float inDelay,
			const PinContextPath& inTargetPortPin);

	virtual bool setFalsePath(
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
			const std::vector<InstContextPath>& inToInstPathsVec);

	virtual bool setMinMaxDelay(
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
			const std::vector<InstContextPath>& inToInstPathsVec);

	virtual bool setMulticyclePath(
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
			const std::vector<InstContextPath>& inToInstPathsVec);

	virtual bool setDisablePinTiming(
			const PinContextPath& inPinPath);

	virtual bool setDisableInstTiming(
			const std::vector<const GenericInst*>& mInstContextVec,
			const GenericPin* inFromPinPtr,
			const GenericPin* inToPinPtr);

	virtual bool setGlobalTimingDerate(
			bool inCellDelay,
			bool inCellCheck,
			bool inNetDelay,
			bool inData,
			bool inClock,
			bool inEarly,
			bool inLate,
			bool inRise,
			bool inFall,
			float inValue);

public:

	virtual bool readSPEFFile(
			const std::string& inFileName);

	virtual bool readSPEFStream(
			std::istream& inDataStream);

	virtual bool setNetsLumpCap(
			const std::vector<NetContextPath>& inNetsDataVec,
			const std::vector<float>& inNetCapsVec);

public:

	virtual bool readSdfFile(
			const std::string& inFileName);

	virtual bool readSdfStream(
			std::istream& inDataStream);

	virtual bool writeSdfFile(
			const std::string& inFileName);

public:

	virtual bool loadNetlistGraph(
			const GenericBlock* inBlockPtr);

	virtual bool loadNetlistSlacks();

	virtual bool getArcCritFactor(
			const GenericPin* inSourcePinPtr,
			const GenericPin* inSinkPinPtr,
			bool inMinConstraint,
			float& outCritFactor);


	virtual bool setInterPinArcDelay(
			const GenericPin* inSourcePinPtr,
			const GenericPin* inSinkPinPtr,
			bool inMin,
			bool inMax,
			float inValue);

	virtual bool setInterPinArcDelays(
			const std::vector<InterPinDelayData>& inArcDelaysVec,
			bool inMin,
			bool inMax);

	virtual bool reportTiming(
			bool inUnique,
			bool inMin,
			bool inMax,
			bool inUnconstrained,
			uint32_t inEndPointsNum,
			uint32_t inGroupsNum,
			std::string& outReportStr);

protected:

	void clearGraphMapping();

	void clearTimingMapping();

	bool addGraphMapping(
			const GenericBlock* inBlockPtr,
			const std::vector<VertexIdData>& inVertexIdToDataVec,
			const std::vector<EdgeIdData>& inEdgeIdToDataVec);

private:

	bool addBlockPinsInNameMap(
			const GenericInst* inParentInstPtr,
			const GenericBlock* inBlockPtr,
			char inDivider,
			const std::string& inPath,
			std::map<std::string, GenericPin*>& outPathToPinMap);

	bool matchVertexPins(
			const std::vector<VertexIdData>& inVertexIdToDataVec,
			const std::map<std::string, GenericPin*>& inPathToPinUMap,
			std::vector<GenericPin*>& outVertexIdToPinVec);

	bool fillPinToVertexIdMaps(
			const std::vector<GenericPin*>& inVertexIdToPinVec,
			const std::vector<VertexIdData>& inVertexIdToDataVec,
			std::unordered_map<const GenericPin*, uint32_t>& mDriverPinToVertexIdUMap,
			std::unordered_map<const GenericPin*, uint32_t>& mSinkPinToVertexIdUMap);

	bool fillPinPairsToEdgeIdMap(
			const std::vector<GenericPin*>& inVertexIdToPinVec,
			const std::vector<EdgeIdData>& inEdgeIdToDataVec,
			PinsPairToEdgeIdUMMap& outPinPairToEdgeIdUMap);

private:

	virtual bool calcNodeCritFactors(
			const std::vector<NodeTimingData>& inNodeTimingData,
			std::vector<float>& outNodeCritFactorsVec,
			bool inMinConstraint);

	virtual float calcCritFactor(
			const NodeTimingData& inData,
			float inGroupMinSlack,
			float inGroupMaxSlack,
			float inGroupMaxRat,
			bool inMinConstraint);

	float getNodeDataSlack(
			const NodeTimingData& inData,
			bool inMinConstraint);

private:
	template<typename _CommandType>
	bool fillPinNetCommand(
			_CommandType& outCommand,
			const std::vector<GenericInst*>& inInstContextVec,
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr);

private:
	template<typename _ObjType>
	bool convertContextObjPath(
			const ObjectContextPath<_ObjType>& inObjPath,
			ObjectContextNameData& outStringPath);

	template<typename _ObjType>
	bool convertContextNameDataVec(
			const std::vector<ObjectContextPath<_ObjType>>& inObjPathsVec,
			std::vector<ObjectContextNameData>& outObjContextPathsVec);



	bool fillPathMesgData(
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
			TimingPathMessageBase& outMessage);

protected:

	friend class StaClientIpcProtocol;


};


/**
 * Fills names of: net, net's parent block, pin, instance;
 */
template<typename _CommandType>
bool StaClientBase::fillPinNetCommand(
		_CommandType& outCommand,
		const std::vector<GenericInst*>& inInstContextVec,
		const GenericPin* inPinPtr,
		const GenericNet* inNetPtr) {
	if(!inPinPtr || !inNetPtr)
		return false;

	GenericBlock* netParentPtr = getParentBlock(inNetPtr);
	GenericInst* instPtr = getParentInstance(inPinPtr);
	if(!netParentPtr || !instPtr)
		return false;

	GenericBlock* instParentPtr = getParentBlock(instPtr);
	if(netParentPtr != instParentPtr)
		return false;

	//outCommand.mParentBlockName = getName(netParentPtr);
	outCommand.mNetName = getName(inNetPtr);
	outCommand.mInstName = getName(instPtr);
	outCommand.mPinName = getName(inPinPtr);

	//then adding names of all instances
	for(auto instPtr : inInstContextVec) {
		if(!instPtr)
			continue;
		outCommand.mInstContextVec.push_back(getName(instPtr));
	}

	return true;
}


/**
 * Fills instance hierarchy and object name of the object.
 * Does nothing and returns false if object pointer is null.
 * @param inObjPath pin path
 * @param outStringPath output path to fill
 */
template<typename _ObjType>
bool StaClientBase::convertContextObjPath(
						const ObjectContextPath<_ObjType>& inObjPath,
						ObjectContextNameData& outStringPath) {
	if(!inObjPath.mObjectPtr)
		return false;

	for(const GenericInst* instPtr : inObjPath.mInstContextVec)
		outStringPath.mInstContextVec.push_back(getName(instPtr));

	outStringPath.mObjName = getName(inObjPath.mObjectPtr);
	return true;
}

/**
 * Converts vector of object paths to name paths.
 * Returns false if fails to fill one of name paths
 */
template<typename _ObjType>
bool StaClientBase::convertContextNameDataVec(
						const std::vector<ObjectContextPath<_ObjType>>& inObjPathsVec,
						std::vector<ObjectContextNameData>& outObjContextPathsVec) {
	bool isOk = true;
	for(const ObjectContextPath<_ObjType>& inData : inObjPathsVec) {
		ObjectContextNameData namePath;
		if(!convertContextObjPath(inData, namePath)) {
			isOk = false;
			continue;
		}

		outObjContextPathsVec.push_back(namePath);
	}

	return isOk;
}




}



#endif /* SRC_NEW_STACLIENTBASE_HPP_ */
