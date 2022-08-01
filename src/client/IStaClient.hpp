#ifndef SRC_NEW_ISTACLIENT_HPP_
#define SRC_NEW_ISTACLIENT_HPP_

#include "GenericNetlistEntities.hpp"

#include <string>
#include <vector>
#include <iostream>


namespace stamask {

/**
 * Delay between two pins.
 * To pass massive amount of delays as argument.
 */
struct InterPinDelayData {
	const GenericPin* mSourcePinPtr = nullptr;
	const GenericPin* mSinkPinPtr = nullptr;
	float mValue = 0;
};

/**
 * Load capacitance of single net.
 * To send capacitances of several nets at a time.
 */
template<typename _ObjType>
struct ObjectContextPath {
	std::vector<const GenericInst*> mInstContextVec;
	const _ObjType* mObjectPtr = nullptr;
};

typedef ObjectContextPath<GenericNet> NetContextPath;
typedef ObjectContextPath<GenericPin> PinContextPath;
typedef ObjectContextPath<GenericInst> InstContextPath;


/**
 * Interface to make a class that is an interface to STA engine.
 * Uses empty generic netlist classes as markers.
 * User is free to inherit them in his own netlist and use it's objects.
 */
class IStaClient {

public:

	/**
	 * Empty destructor.
	 */
	virtual ~IStaClient() {}

	/**
	 * Callback method to get errors from interface.
	 * @param inErrorStr error message
	 */
	virtual void printError(
			const std::string& inErrorStr) = 0;

	/**
	 * Method to set hierarchy separator in STA.
	 * @param inSeparator separator
	 */
	virtual void setHierarchySeparator(
			char inSeparator) = 0;

	/**
	 * Method to exit STA.
	 */
	virtual bool exit() = 0;

	/**
	 * Method to check that STA server responds.
	 */
	virtual bool ping(
			unsigned long inMsTimeout) = 0;


	/**
	 * Method to read liberty file.
	 * @param inFileName file name
	 * @return success status
	 */
	virtual bool readLibertyFile(
			const std::string& inFileName) = 0;
	/**
	 * Method to read liberty from stream.
	 * @param inDataStream stream to read
	 * @return success status
	 */
	virtual bool readLibertyStream(
			std::istream& inDataStream) = 0;
	/**
	 * Method to clear out all stored liberty libraries from STA.
	 * @return success status
	 */
	virtual bool clearLibraries() = 0;


	/**
	 * Method to read Verilog file.
	 * @param inFileName file name
	 * @return success status
	 */
	virtual bool readVerilogFile(
			const std::string& inFileName) = 0;
	/**
	 * Method to read Verilog from stream.
	 * @param inDataStream stream to read
	 * @return success status
	 */
	virtual bool readVerilogStream(
			std::istream& inDataStream) = 0;

	/**
	 * Method to build netlist structures from verilog data with given top-block.
	 * @param inTopBlockName top block name
	 * @return success status
	 */
	virtual bool linkVerilogTopBlockNetlist(
			const std::string& inTopBlockName) = 0;



	/**
	 * Method to build anew netlist structures with given top-block in STA.
	 * @param inBlockPtr top block
	 * @return success status
	 */
	virtual bool linkCreateTopBlockNetlist(
			const GenericBlock* inBlockPtr) = 0;


	/**
	 * Method to get mappings of nodes and edges of timing graph from STA engine.
	 * This is useful to interact with engine using netlist objects as markers,
	 * not nodes and edges.
	 * @param inBlockPtr top block
	 * @return success status
	 */
	virtual bool loadNetlistGraph(
			const GenericBlock* inBlockPtr) = 0;




	/**
	 * Method to connect pin and net from top-block in STA.
	 * @param inPinPtr pin pointer
	 * @param inNetPtr net pointer
	 * @return success status
	 */
	virtual bool connectTopContextPinNet(
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr) = 0;

	/**
	 * Method to disconnect pin and net from top-block in STA.
	 * @param inPinPtr pin pointer
	 * @param inNetPtr net pointer
	 * @return success status
	 */
	virtual bool disconnectTopContextPinNet(
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr) = 0;

	/**
	 * Method to connect pin and net from inst-context in STA.
	 * @param inInstContextVec top-bottom sequence of instances
	 * @param inPinPtr pin pointer
	 * @param inNetPtr net pointer
	 * @return success status
	 */
	virtual bool connectContextPinNet(
			const std::vector<GenericInst*>& inInstContextVec,
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr) = 0;

	/**
	 * Method to disconnect pin and net from inst-context in STA.
	 * @param inInstContextVec top-bottom sequence of instances
	 * @param inPinPtr pin pointer
	 * @param inNetPtr net pointer
	 * @return success status
	 */
	virtual bool disconnectContextPinNet(
			const std::vector<GenericInst*>& inInstContextVec,
			const GenericPin* inPinPtr,
			const GenericNet* inNetPtr) = 0;


	/**
	 * Method to clear current netlist from STA.
	 * @return success status
	 */
	virtual bool clearNetlistBlocks() = 0;

public:

	/**
	 * Adds clock creation constraint.
	 * @param inName clock name
	 * @param inDescription clock comment
	 * @param inPinPathsVec source pin paths
	 * @param inAddFlag add flag
	 * @param inPeriod clock period
	 * @param inWaveformVec clock waveform
	 * @return operation success
	 */
	virtual bool createClock(
			const std::string& inName,
			const std::string& inDescription,
			const std::vector<PinContextPath>& inPinPathsVec,
			bool inAddFlag,
			float inPeriod,
			const std::vector<float>& inWaveformVec) = 0;


	/**
	 * Adds generate clock creation constraint.
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
			const std::vector<float>& inEdgeShiftsVec) = 0;

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
	virtual bool setClockGroups(
			const std::string& inName,
			const std::string& inDescription,
			bool inLogicalExclusive,
			bool inPhysicalExclusive,
			bool inAsynchronous,
			bool inAllowPaths,
			const std::vector<std::vector<std::string>>& inClockGroupsVec) = 0;


	/**
	 * Adds constraint with clock source/network latency.
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
			const PinContextPath& inPinPath) = 0;


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
	virtual bool setClockToClockUncertainty(
			const std::string& inFromClockName,
			bool inFromRise,
			bool inFromFall,
			const std::string& inToClockName,
			bool inToRise,
			bool inToFall,
			bool inSetup,
			bool inHold,
			float inValue) = 0;

	/**
	 * Adds uncertainty constraint of single clock.
	 * @param inClockName clock name
	 * @param inSetup setup time flag
	 * @param inHold hold time flag
	 * @param inValue value to set
	 * @return operations success
	 */
	virtual bool setClockUncertainty(
			const std::string& inClockName,
			bool inSetup,
			bool inHold,
			float inValue) = 0;

	/**
	 * Adds clock uncertainty constraint of single pin.
	 * @param inPinPath pin path
	 * @param inSetup setup time flag
	 * @param inHold hold time flag
	 * @param inValue value to set
	 * @return operations success
	 */
	virtual bool setPinUncertainty(
			const PinContextPath& inPinPath,
			bool inSetup,
			bool inHold,
			float inValue) = 0;

	/**
	 * Adds delay of input/output port respective to clock.
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
			const PinContextPath& inTargetPortPin) = 0;

	/**
	 * Adds edge transition time of input port's signal.
	 * @param inDelayRise apply transition for rising edges
	 * @param inDelayFall apply transition for falling edges
	 * @param inDelayMax delay is max delay
	 * @param inDelayMin delay is max delay
	 * @param inDelay delay value
	 * @param inTargetPortPin target port
	 * @return operations success
	 */
	virtual bool setInputTransition(
			bool inDelayRise,
			bool inDelayFall,
			bool inDelayMax,
			bool inDelayMin,
			float inDelay,
			const PinContextPath& inTargetPortPin) = 0;


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
	 * @param inRise end point rise transition
	 * @param inFall end point fall transition
	 * @return operation success
	 */
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
		const std::vector<InstContextPath>& inToInstPathsVec,
		bool inRise,
		bool inFall) = 0;



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
	 * @param inRise end point rise transition
	 * @param inFall end point fall transition
	 * @return operation success
	 */
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
			const std::vector<InstContextPath>& inToInstPathsVec,
			bool inRise,
			bool inFall) = 0;

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
	 * @param inRise end point rise transition
	 * @param inFall end point fall transition
	 * @return operation success
	 */
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
			const std::vector<InstContextPath>& inToInstPathsVec,
			bool inRise,
			bool inFall) = 0;


	/**
	 * Adds constraint to disable timing arcs of portref.
	 * @param inPinPath target pin path
	 * @return operation success
	 */
	virtual bool setDisablePinTiming(
			const PinContextPath& inPinPath) = 0;

	/**
	 * Adds constraint to disable timing arcs of instance.
	 * @param inFromPinName path of from-pin
	 * @param inToPinName path of to-pin
	 * @return operation success
	 */
	virtual bool setDisableInstTiming(
			const std::vector<const GenericInst*>& mInstContextVec,
			const GenericPin* inFromPinPtr,
			const GenericPin* inToPinPtr) = 0;

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
			float inValue) = 0;


public:
	/**
	 * Method to read SPEF file.
	 * @param inFileName file name
	 * @return success status
	 */
	virtual bool readSPEFFile(
			const std::string& inFileName) = 0;
	/**
	 * Method to read SPEF stream.
	 * @param inDataStream data stream
	 * @return success status
	 */
	virtual bool readSPEFStream(
			std::istream& inDataStream) = 0;

	/**
	 * Sets lumped capacity for several nets in specific hierarchical context.
	 * @param inNetsDataVec nets and their context
	 * @param inNetCapsVec capacitance values
	 * @return success status
	 */
	virtual bool setNetsLumpCap(
			const std::vector<NetContextPath>& inNetsDataVec,
			const std::vector<float>& inNetCapsVec) = 0;

public:

	/**
	 * Method to read SDF file.
	 * @param inFileName file name
	 * @return success status
	 */
	virtual bool readSdfFile(
			const std::string& inFileName) = 0;
	/**
	 * Method to read SDF stream.
	 * @param inDataStream data stream
	 * @return success status
	 */
	virtual bool readSdfStream(
			std::istream& inDataStream) = 0;

	/**
	 * Sends command to write SDF file.
	 * @param inFileName file name
	 * @return success status
	 */
	virtual bool writeSdfFile(
			const std::string& inFileName) = 0;

public:

	/**
	 * Method to set inter-pin delay in top-module context.
	 * @param inSourcePinPtr source pin pointer
	 * @param inSinkPinPtr sink pin pointer
	 * @param inMin apply as min delay
	 * @param inMax apply as max delay
	 * @param inValue delay value
	 */
	virtual bool setInterPinArcDelay(
			const GenericPin* inSourcePinPtr,
			const GenericPin* inSinkPinPtr,
			bool inMin,
			bool inMax,
			float inValue) = 0;


	/**
	 * Method to set inter-pin delays in top-module context.
	 * @param inArcDelaysVec pin pairs and their delays
	 * @param inMin apply as min delay
	 * @param inMax apply as max delay
	 */
	virtual bool setInterPinArcDelays(
			const std::vector<InterPinDelayData>& inArcDelaysVec,
			bool inMin,
			bool inMax) = 0;

public:

	/**
	 * Method to report timing with given amount of critical paths.
	 * @param inUnique report unique paths(without rise/fall variations)
	 * @param inMin report min-constraint paths
	 * @param inMax report max-constraint paths
	 * @param inUnconstrained report unconstrained paths
	 * @param inEndPointsNum amount of endpoints to report
	 * @param inGroupsNum amount of groups to report
	 * @param outReportStr report string to fill
	 * @return success status
	 */
	virtual bool reportTiming(
			bool inUnique,
			bool inMin,
			bool inMax,
			bool inUnconstrained,
			uint32_t inEndPointsNum,
			uint32_t inGroupsNum,
			std::string& outReportStr) = 0;

	/**
	 * Method to report basic timing stats of the design.
	 * @param inCommand command to execute
	 * @param outMinWNS worst slack for min condition
	 * @param outMinWNS worst slack for max condition
	 * @param outMinWNS total negative slack for min condition
	 * @param outMinWNS total negative slack for max condition
	 * @success status
	 */
	virtual bool getDesignStats(
			float& outMinWNS,
			float& outMaxWNS,
			float& outMinTNS,
			float& outMaxTNS) = 0;

};


}

#endif /* SRC_NEW_ISTACLIENT_HPP_ */
