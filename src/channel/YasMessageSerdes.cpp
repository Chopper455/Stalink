
#include "YasMessageSerdes.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "yas/mem_streams.hpp"
#include "yas/binary_iarchive.hpp"
#include "yas/binary_oarchive.hpp"
#include "yas/types/std/vector.hpp"
#include "yas/types/std/string.hpp"
#include "yas/types/std/map.hpp"
#include "yas/types/std/pair.hpp"



namespace yas {


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandExit &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandPing &inObj) {
	outArch & inObj.mStr & inObj.mMsTimeout;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetHierarhySeparator &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadLibertyFile &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadLibertyStream &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandClearLibs &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadVerilogFile &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadVerilogStream &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandLinkTop &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandClearNetlistBlocks &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::PortData &inObj) {
	outArch &
		inObj.mName &
		inObj.mInput &
		inObj.mOutput &
		inObj.mBusFlag &
		inObj.mRangeFrom &
		inObj.mRangeTo &
		inObj.mConnNetIdxsVec;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::InstanceData &inObj) {
	outArch &
		inObj.mName &
		inObj.mMasterBlockIdx &
		inObj.mPortDataVec;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::BlockData &inObj) {
	outArch &
		inObj.mName &
		inObj.mTopFlag &
		inObj.mLeafFlag &
		inObj.mPortDataVec &
		inObj.mInstDataVec &
		inObj.mNetNamesVec &
		inObj.mGndNetName &
		inObj.mVddNetName;
}



template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandCreateNetlist &inObj) {
	outArch &
		inObj.mBlockDataVec;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandGetGraphData &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandGetGraphSlacksData &inObj) {
	outArch & inObj.mStr;
}



template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandConnectContextPinNet &inObj) {
	outArch &
		inObj.mInstContextVec &
		inObj.mInstName &
		inObj.mPinName &
		inObj.mNetName;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandDisconnectContextPinNet &inObj) {
	outArch &
		inObj.mInstContextVec &
		inObj.mInstName &
		inObj.mPinName &
		inObj.mNetName;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadSpefFile &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadSpefStream &inObj) {
	outArch & inObj.mStr;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::ObjectContextNameData &inObj) {
	outArch &
	inObj.mInstContextVec &
	inObj.mObjName;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetGroupNetCap &inObj) {
	outArch &
	inObj.mNetAddrsVec &
	inObj.mValuesVec;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadSdfFile &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReadSdfStream &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandWriteSdfFile &inObj) {
	outArch & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetArcsDelays &inObj) {
	outArch &
		inObj.mDelayValuesVec &
		inObj.mEdgeIdsVec &
		inObj.mMin &
		inObj.mMax;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandReportTiming &inObj) {
	outArch &
		inObj.mUniquePaths &
		inObj.mMin &
		inObj.mMax &
		inObj.mUnconstrained &
		inObj.mEndPointsNum &
		inObj.mGroupsNum;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandGetDesignStats &inObj) {
	outArch & inObj.mStr;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandCreateClock &inObj) {
	outArch &
		inObj.mName &
		inObj.mDescription &
		inObj.mPinPathsVec &
		inObj.mAddFlag &
		inObj.mPeriod &
		inObj.mWaveformVec;
}



template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandCreateGenClock &inObj) {
	outArch &
		inObj.mName &
		inObj.mDescription &
		inObj.mMasterClockPinPath &
		inObj.mMasterClockName &
		inObj.mPinPathsVec &
		inObj.mAddFlag &
		inObj.mDivideFactor &
		inObj.mMultiplyFactor &
		inObj.mDutyCycle &
		inObj.mInvert &
		inObj.mEdgesVec &
		inObj.mEdgeShiftsVec;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetClockGroups &inObj) {
	outArch &
		inObj.mName &
		inObj.mDescription &
		inObj.mLogicalExclusive &
		inObj.mPhysicalExclusive &
		inObj.mAsynchronous &
		inObj.mAllowPaths &
		inObj.mClockGroupsVec;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetClockLatency &inObj) {
	outArch &
		inObj.mSource &
		inObj.mMin &
		inObj.mMax &
		inObj.mEarly &
		inObj.mLate &
		inObj.mRise &
		inObj.mFall &
		inObj.mValue &
		inObj.mClockName &
		inObj.mPinPath;
}



template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetInterClockUncertainty &inObj) {
	outArch &
		inObj.mFromClockName &
		inObj.mFromRise &
		inObj.mFromFall &
		inObj.mToClockName &
		inObj.mToRise &
		inObj.mToFall &
		inObj.mSetup &
		inObj.mHold &
		inObj.mValue;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetSingleClockUncertainty &inObj) {
	outArch &
		inObj.mClockName &
		inObj.mSetup &
		inObj.mHold &
		inObj.mValue;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetSinglePinUncertainty &inObj) {
	outArch &
		inObj.mPinPath &
		inObj.mSetup &
		inObj.mHold &
		inObj.mValue;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetPortDelay &inObj) {
	outArch &
		inObj.mIsInput &
		inObj.mClockName &
		inObj.mClockPinPath &
		inObj.mClockFall &
		inObj.mLevelSensitive &
		inObj.mDelayRise &
		inObj.mDelayFall &
		inObj.mDelayMax &
		inObj.mDelayMin &
		inObj.mAdd &
		inObj.mNetworkLatencyInc &
		inObj.mSourceLatencyInc &
		inObj.mDelay &
		inObj.mTargetPortPin;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetInPortTransition &inObj) {
	outArch &
		inObj.mDelayRise &
		inObj.mDelayFall &
		inObj.mDelayMax &
		inObj.mDelayMin &
		inObj.mValue &
		inObj.mTargetPortPin;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetFalsePath &inObj) {
	outArch &
		inObj.mFromRise &
		inObj.mFromFall &
		inObj.mFromPinPathsVec &
		inObj.mFromClocksVec &
		inObj.mFromInstPathsVec &
		inObj.mThroughRise &
		inObj.mThroughFall &
		inObj.mThroughPinPathsVec &
		inObj.mThroughInstPathsVec &
		inObj.mThroughNetPathsVec &
		inObj.mToRise &
		inObj.mToFall &
		inObj.mToPinPathsVec &
		inObj.mToClocksVec &
		inObj.mToInstPathsVec &
		inObj.mSetup &
		inObj.mHold &
		inObj.mComment;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetMinMaxDelay &inObj) {
	outArch &
		inObj.mFromRise &
		inObj.mFromFall &
		inObj.mFromPinPathsVec &
		inObj.mFromClocksVec &
		inObj.mFromInstPathsVec &
		inObj.mThroughRise &
		inObj.mThroughFall &
		inObj.mThroughPinPathsVec &
		inObj.mThroughInstPathsVec &
		inObj.mThroughNetPathsVec &
		inObj.mToRise &
		inObj.mToFall &
		inObj.mToPinPathsVec &
		inObj.mToClocksVec &
		inObj.mToInstPathsVec &
		inObj.mMinDelayFlag &
		inObj.mValue &
		inObj.mComment;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetMulticyclePath &inObj) {
	outArch &
		inObj.mFromRise &
		inObj.mFromFall &
		inObj.mFromPinPathsVec &
		inObj.mFromClocksVec &
		inObj.mFromInstPathsVec &
		inObj.mThroughRise &
		inObj.mThroughFall &
		inObj.mThroughPinPathsVec &
		inObj.mThroughInstPathsVec &
		inObj.mThroughNetPathsVec &
		inObj.mToRise &
		inObj.mToFall &
		inObj.mToPinPathsVec &
		inObj.mToClocksVec &
		inObj.mToInstPathsVec &
		inObj.mSetup &
		inObj.mHold &
		inObj.mStart &
		inObj.mEnd &
		inObj.mValue &
		inObj.mComment;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandDisableSinglePinTiming &inObj) {
	outArch &
		inObj.mPinPath;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandDisableInstTiming &inObj) {
	outArch &
		inObj.mInstContextVec &
		inObj.mFromPinName &
		inObj.mToPinName;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::CommandSetGlobalTimingDerate &inObj) {
	outArch &
		inObj.mCellDelay &
		inObj.mCellCheck &
		inObj.mNetDelay &
		inObj.mData &
		inObj.mClock &
		inObj.mEarly &
		inObj.mLate &
		inObj.mRise &
		inObj.mFall &
		inObj.mValue;
}



template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::ResponseCommExecStatus &inObj) {
	outArch & inObj.mExecStatus & inObj.mStr;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::EdgeIdData &inObj) {
	outArch & inObj.mEdgeId &
		inObj.mFromVertexId &
		inObj.mToVertexId;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::VertexIdData &inObj) {
	outArch & inObj.mContextInstNamesVec &
		inObj.mIsDriver &
		inObj.mPinName &
		inObj.mVertexId;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::ResponseGraphMap &inObj) {
	outArch & inObj.mExecStatus &
		inObj.mStr &
		inObj.mVertexIdToDataVec &
		inObj.mEdgeIdToDataVec;
}


template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::NodeTimingData &inObj) {
	outArch &
		inObj.mClkIdx &
		inObj.mEndPointIdx &
		inObj.mHasEndMaxPathRat &
		inObj.mHasEndMinPathRat &
		inObj.mHasTiming &
		inObj.mNonData &
		inObj.mIsEndPoint &
		inObj.mMaxPathRat &
		inObj.mMinPathRat &
		inObj.mNodeId &
		inObj.mMaxWorstSlackAat &
		inObj.mMaxWorstSlackRat &
		inObj.mMinWorstSlackAat &
		inObj.mMinWorstSlackRat;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::ResponseGraphSlacks &inObj) {
	outArch & inObj.mExecStatus &
		inObj.mStr &
		inObj.mNodeTimingsVec;
}

template<typename _ArchiveType>
void serialize(
		_ArchiveType& outArch,
		stamask::ResponseDesignStats &inObj) {
	outArch &
		inObj.mExecStatus &
		inObj.mStr &
		inObj.mMaxTNS &
		inObj.mMaxWslack &
		inObj.mMinTNS &
		inObj.mMinWslack;
}


}





namespace stamask {


/**
 * Default constructor.
 */
YasMessageSerdes::YasMessageSerdes():
		AbsMessageSerdes(),
		mDataBuf() {}

/**
 * Empty destructor.
 */
YasMessageSerdes::~YasMessageSerdes() {}

/**
 * Returns encoder ID.
 * @return 41
 */
uint32_t YasMessageSerdes::getEncoderId() const {
	return 41;
}

/**
 * Serializes message and returns block with data pointer.
 * Switches message type and casts to target the message.
 * Returns data block with nullptr if message type isn't supported.
 * @param inMessage target message
 * @return data block
 */
DataBlock YasMessageSerdes::serializeMessage(
		const Message& inMessage) {

	switch(inMessage.getMesgType()) {
	case EMessageType::EMessageTypeExit:
		return serialize((const CommandExit&)inMessage);

	case EMessageType::EMessageTypePing:
		return serialize((const CommandPing&)inMessage);

	case EMessageType::EMessageTypeSetHierSeparator:
		return serialize((const CommandSetHierarhySeparator&)inMessage);

	case EMessageType::EMessageTypeReadLibFile:
		return serialize((const CommandReadLibertyFile&)inMessage);

	case EMessageType::EMessageTypeReadLibStream:
		return serialize((const CommandReadLibertyStream&)inMessage);

	case EMessageType::EMessageTypeClearLibs:
		return serialize((const CommandClearLibs&)inMessage);

	case EMessageType::EMessageTypeReadVerilogFile:
		return serialize((const CommandReadVerilogFile&)inMessage);

	case EMessageType::EMessageTypeReadVerilogStream:
		return serialize((const CommandReadVerilogStream&)inMessage);

	case EMessageType::EMessageTypeLinkTop:
		return serialize((const CommandLinkTop&)inMessage);

	case EMessageType::EMessageTypeClearNetlistBlocks:
		return serialize((const CommandClearNetlistBlocks&)inMessage);

	case EMessageType::EMessageTypeCreateNetlist:
		return serialize((CommandCreateNetlist&)inMessage);

	case EMessageType::EMessageTypeGetGraphData:
		return serialize((const CommandGetGraphData&)inMessage);

	case EMessageType::EMessageTypeGetGraphSlacksData:
		return serialize((const CommandGetGraphSlacksData&)inMessage);

	case EMessageType::EMessageTypeConnectContextPinNet:
		return serialize((const CommandConnectContextPinNet&)inMessage);

	case EMessageType::EMessageTypeDisconnectContextPinNet:
		return serialize((const CommandDisconnectContextPinNet&)inMessage);

	case EMessageType::EMessageTypeReadSpefFile:
		return serialize((const CommandReadSpefFile&)inMessage);

	case EMessageType::EMessageTypeReadSpefStream:
		return serialize((const CommandReadSpefStream&)inMessage);

	case EMessageType::EMessageTypeSetGroupNetLumpCap:
		return serialize((const CommandSetGroupNetCap&)inMessage);

	case EMessageType::EMessageTypeReadSdfFile:
		return serialize((const CommandReadSdfFile&)inMessage);

	case EMessageType::EMessageTypeReadSdfStream:
		return serialize((const CommandReadSdfStream&)inMessage);

	case EMessageType::EMessageTypeWriteSdfFile:
		return serialize((const CommandWriteSdfFile&)inMessage);

	case EMessageType::EMessageTypeSetArcsDelay:
		return serialize((const CommandSetArcsDelays&)inMessage);

	case EMessageType::EMessageTypeCreateClock:
		return serialize((const CommandCreateClock&)inMessage);

	case EMessageType::EMessageTypeCreateGeneratedClock:
		return serialize((const CommandCreateGenClock&)inMessage);

	case EMessageType::EMessageTypeSetClockGroups:
		return serialize((const CommandSetClockGroups&)inMessage);

	case EMessageType::EMessageTypeSetClockLatency:
		return serialize((const CommandSetClockLatency&)inMessage);

	case EMessageType::EMessageTypeSetInterClockUncertainty:
		return serialize((const CommandSetInterClockUncertainty&)inMessage);

	case EMessageType::EMessageTypeSetSingleClockUncertainty:
		return serialize((const CommandSetSingleClockUncertainty&)inMessage);

	case EMessageType::EMessageTypeSetSinglePinUncertainty:
		return serialize((const CommandSetSinglePinUncertainty&)inMessage);

	case EMessageType::EMessageTypeSetSinglePortDelay:
		return serialize((const CommandSetPortDelay&)inMessage);

	case EMessageType::EMessageTypeSetInPortTransition:
		return serialize((const CommandSetInPortTransition&)inMessage);

	case EMessageType::EMessageTypeSetFalsePath:
		return serialize((const CommandSetFalsePath&)inMessage);

	case EMessageType::EMessageTypeSetMinMaxDelay:
		return serialize((const CommandSetMinMaxDelay&)inMessage);

	case EMessageType::EMessageTypeSetMulticyclePath:
		return serialize((const CommandSetMulticyclePath&)inMessage);

	case EMessageType::EMessageTypeDisableSinglePinTiming:
		return serialize((const CommandDisableSinglePinTiming&)inMessage);

	case EMessageType::EMessageTypeDisableInstTiming:
		return serialize((const CommandDisableInstTiming&)inMessage);

	case EMessageType::EMessageTypeSetGlobalTimingDerate:
		return serialize((const CommandSetGlobalTimingDerate&)inMessage);

	case EMessageType::EMessageTypeReportTiming:
		return serialize((const CommandReportTiming&)inMessage);

	case EMessageType::EMessageTypeGetDesignStats:
		return serialize((CommandGetDesignStats&)inMessage);

	case EMessageType::EMessageTypeExecutionStatus:
		return serialize((const ResponseCommExecStatus&)inMessage);

	case EMessageType::EMessageTypeGraphMap:
		return serialize((const ResponseGraphMap&)inMessage);

	case EMessageType::EMessageTypeGraphSlacks:
		return serialize((const ResponseGraphSlacks&)inMessage);

	case EMessageType::EMessageTypeDesignStats:
		return serialize((const ResponseDesignStats&)inMessage);

	}

	return {nullptr, 0};
}

/**
 * Deserializes message from data block and sets data in message.
 * Switches message type and casts to target the message.
 * Returns false if message type isn't supported.
 * @param outMessage message to fill
 * @param inData data to process
 * @return operation success
 */
bool YasMessageSerdes::deserializeMessage(
		Message& outMessage,
		DataBlock inData) {

	switch(outMessage.getMesgType()) {
	case EMessageType::EMessageTypeExit:
		return deserialize((CommandExit&)outMessage, inData);

	case EMessageType::EMessageTypePing:
		return deserialize((CommandPing&)outMessage, inData);

	case EMessageType::EMessageTypeSetHierSeparator:
		return deserialize((CommandSetHierarhySeparator&)outMessage, inData);

	case EMessageType::EMessageTypeReadLibFile:
		return deserialize((CommandReadLibertyFile&)outMessage, inData);

	case EMessageType::EMessageTypeReadLibStream:
		return deserialize((CommandReadLibertyStream&)outMessage, inData);

	case EMessageType::EMessageTypeClearLibs:
		return deserialize((CommandClearLibs&)outMessage, inData);

	case EMessageType::EMessageTypeReadVerilogFile:
		return deserialize((CommandReadVerilogFile&)outMessage, inData);

	case EMessageType::EMessageTypeReadVerilogStream:
		return deserialize((CommandReadVerilogStream&)outMessage, inData);

	case EMessageType::EMessageTypeLinkTop:
		return deserialize((CommandLinkTop&)outMessage, inData);

	case EMessageType::EMessageTypeClearNetlistBlocks:
		return deserialize((CommandClearNetlistBlocks&)outMessage, inData);

	case EMessageType::EMessageTypeCreateNetlist:
		return deserialize((CommandCreateNetlist&)outMessage, inData);

	case EMessageType::EMessageTypeGetGraphData:
		return deserialize((CommandGetGraphData&)outMessage, inData);

	case EMessageType::EMessageTypeGetGraphSlacksData:
		return deserialize((CommandGetGraphSlacksData&)outMessage, inData);

	case EMessageType::EMessageTypeConnectContextPinNet:
		return deserialize((CommandConnectContextPinNet&)outMessage, inData);

	case EMessageType::EMessageTypeDisconnectContextPinNet:
		return deserialize((CommandDisconnectContextPinNet&)outMessage, inData);

	case EMessageType::EMessageTypeReadSpefFile:
		return deserialize((CommandReadSpefFile&)outMessage, inData);

	case EMessageType::EMessageTypeReadSpefStream:
		return deserialize((CommandReadSpefStream&)outMessage, inData);

	case EMessageType::EMessageTypeSetGroupNetLumpCap:
		return deserialize((CommandSetGroupNetCap&)outMessage, inData);

	case EMessageType::EMessageTypeReadSdfFile:
		return deserialize((CommandReadSdfFile&)outMessage, inData);

	case EMessageType::EMessageTypeReadSdfStream:
		return deserialize((CommandReadSdfStream&)outMessage, inData);

	case EMessageType::EMessageTypeWriteSdfFile:
		return deserialize((CommandWriteSdfFile&)outMessage, inData);

	case EMessageType::EMessageTypeSetArcsDelay:
		return deserialize((CommandSetArcsDelays&)outMessage, inData);

	case EMessageType::EMessageTypeCreateClock:
		return deserialize((CommandCreateClock&)outMessage, inData);

	case EMessageType::EMessageTypeCreateGeneratedClock:
		return deserialize((CommandCreateGenClock&)outMessage, inData);

	case EMessageType::EMessageTypeSetClockGroups:
		return deserialize((CommandSetClockGroups&)outMessage, inData);

	case EMessageType::EMessageTypeSetClockLatency:
		return deserialize((CommandSetClockLatency&)outMessage, inData);

	case EMessageType::EMessageTypeSetInterClockUncertainty:
		return deserialize((CommandSetInterClockUncertainty&)outMessage, inData);

	case EMessageType::EMessageTypeSetSingleClockUncertainty:
		return deserialize((CommandSetSingleClockUncertainty&)outMessage, inData);

	case EMessageType::EMessageTypeSetSinglePinUncertainty:
		return deserialize((CommandSetSinglePinUncertainty&)outMessage, inData);

	case EMessageType::EMessageTypeSetSinglePortDelay:
		return deserialize((CommandSetPortDelay&)outMessage, inData);

	case EMessageType::EMessageTypeSetInPortTransition:
		return deserialize((CommandSetInPortTransition&)outMessage, inData);

	case EMessageType::EMessageTypeSetFalsePath:
		return deserialize((CommandSetFalsePath&)outMessage, inData);

	case EMessageType::EMessageTypeSetMinMaxDelay:
		return deserialize((CommandSetMinMaxDelay&)outMessage, inData);

	case EMessageType::EMessageTypeSetMulticyclePath:
		return deserialize((CommandSetMulticyclePath&)outMessage, inData);

	case EMessageType::EMessageTypeDisableSinglePinTiming:
		return deserialize((CommandDisableSinglePinTiming&)outMessage, inData);

	case EMessageType::EMessageTypeDisableInstTiming:
		return deserialize((CommandDisableInstTiming&)outMessage, inData);

	case EMessageType::EMessageTypeSetGlobalTimingDerate:
		return deserialize((CommandSetGlobalTimingDerate&)outMessage, inData);

	case EMessageType::EMessageTypeReportTiming:
		return deserialize((CommandReportTiming&)outMessage, inData);

	case EMessageType::EMessageTypeGetDesignStats:
		return deserialize((CommandGetDesignStats&)outMessage, inData);

	case EMessageType::EMessageTypeExecutionStatus:
		return deserialize((ResponseCommExecStatus&)outMessage, inData);

	case EMessageType::EMessageTypeGraphMap:
		return deserialize((ResponseGraphMap&)outMessage, inData);

	case EMessageType::EMessageTypeGraphSlacks:
		return deserialize((ResponseGraphSlacks&)outMessage, inData);

	case EMessageType::EMessageTypeDesignStats:
		return deserialize((ResponseDesignStats&)outMessage, inData);
	}

	return false;
}



}



