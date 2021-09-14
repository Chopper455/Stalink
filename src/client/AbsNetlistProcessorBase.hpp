#ifndef SRC_CLIENT_ABSNETLISTPROCESSORBASE_HPP_
#define SRC_CLIENT_ABSNETLISTPROCESSORBASE_HPP_


#include "AbsNetlistProcessorBase.hpp"
#include "GenericNetlistEntities.hpp"
#include "channel/Messages.hpp"

#include <unordered_map>
#include <map>
#include <vector>
#include <limits>

namespace stamask {

/**
 * Base class that provides interface for subclasses
 * to operate on netlist objects.
 */
class AbsNetlistProcessorBase {

protected:

	typedef std::unordered_map<const GenericBlock*, uint32_t>
			BlockPtrToIdUMap;
	typedef std::map<std::string, uint32_t>
			StringToIdMap;

protected:

	AbsNetlistProcessorBase();

	virtual ~AbsNetlistProcessorBase();


	//methods to get necessary data from the netlist objects

	virtual std::string getName(
			const GenericBlock* inBlockPtr) const = 0;
	virtual std::string getName(
			const GenericInst* inInstPtr) const = 0;
	virtual std::string getName(
			const GenericNet* inNetPtr) const = 0;
	virtual std::string getName(
			const GenericPin* inPinPtr) const = 0;
	virtual std::string getName(
			const GenericPort* inPortPtr) const = 0;

	virtual bool isInput(
			const GenericPort* inPortPtr) const = 0;
	virtual bool isOutput(
			const GenericPort* inPortPtr) const = 0;

	virtual bool isInput(
			const GenericPin* inPinPtr) const = 0;
	virtual bool isOutput(
			const GenericPin* inPinPtr) const = 0;

	virtual GenericInst* getParentInstance(
			const GenericPin* inPinPtr) const = 0;
	virtual stamask::GenericInst* getParentInstance(
			const stamask::GenericInst* inInstPtr) const = 0;
	virtual stamask::GenericInst* getParentInstance(
			const stamask::GenericNet* inNetPtr) const = 0;


	virtual void getPorts(
			const GenericBlock* inBlockPtr,
			std::vector<GenericPort*>& outPortsVec) = 0;
	virtual void getPorts(
			const GenericInst* inInstPtr,
			std::vector<GenericPin*>& outPinsVec) = 0;

	//to allow processing of two different netlist structures
	virtual void getBlockNets(
			const GenericInst* inParentInstPtr,
			const GenericBlock* inBlockPtr,
			std::vector<GenericNet*>& outNetsVec) = 0;

	virtual void getBlockInsts(
			const GenericInst* inParentInstPtr,
			const GenericBlock* inBlockPtr,
			std::vector<GenericInst*>& outInstsVec) = 0;


	virtual bool isLeafBlock(
			const GenericBlock* inBlockPtr) const = 0;
	virtual GenericBlock* getMasterBlock(
			const GenericInst* inInstPtr) const = 0;
	virtual GenericBlock* getParentBlock(
			const GenericInst* inInstPtr) const = 0;

	virtual GenericBlock* getParentBlock(
			const GenericNet* inNetPtr) const = 0;

	virtual void getInstPins(
			const GenericInst* inInstPtr,
			std::vector<GenericPin*>& outPinsVec) = 0;
	virtual GenericPin* getPortPin(
			const GenericPort* inPortPtr) const = 0;

	virtual bool isBus(const GenericPort* inBlockPtr) const = 0;
	virtual bool isBit(const GenericPort* inBlockPtr) const = 0;
	virtual  GenericPort* getBit(
			const GenericPort* inBlockPtr,
			uint32_t inBitIdx) const = 0;

	virtual bool isBus(const GenericPin* inBlockPtr) const = 0;
	virtual bool isBit(const GenericPin* inBlockPtr) const = 0;
	virtual GenericPin* getBit(
			const GenericPin* inBlockPtr,
			uint32_t inBitIdx) const = 0;

	virtual bool isBus(const GenericNet* inNetPtr) const = 0;

	virtual GenericNet* getConnectedNet(
			const GenericPort* inBlockPtr) const = 0;
	virtual GenericNet* getConnectedNet(
			const GenericPin* inBlockPtr) const = 0;

	virtual bool hasGndSource(
			const GenericNet* inNetPtr) const = 0;
	virtual bool hasVddSource(
			const GenericNet* inNetPtr) const = 0;

	virtual uint32_t getBusRangeFrom(const GenericPort* inBlockPtr) const = 0;
	virtual uint32_t getBusRangeTo(const GenericPort* inBlockPtr) const = 0;

	virtual uint32_t getBusRangeFrom(const GenericPin* inBlockPtr) const = 0;
	virtual uint32_t getBusRangeTo(const GenericPin* inBlockPtr) const = 0;

protected:


	template<typename _ObjectType, typename _DataType, typename _PortType>
	bool fillObjectPorts(
			_ObjectType* inBlockPtr,
			_DataType& ioBlockData,
			const StringToIdMap& inNetNameToIdxMap,
			bool inIgnoreBits);

	template<typename _ObjectType>
	bool fillNetsConnVec(
			_ObjectType* inObjectPtr,
			const StringToIdMap& inNetNameToIdxMap,
			std::vector<uint32_t>& outNetIdxVec);

	template<typename _ObjectType>
	bool getConnectedNetIdx(
			_ObjectType* inObjectPtr,
			const StringToIdMap& inNetNameToIdxMap,
			uint32_t& outNetIdx);



	bool fillBlockData(
			const GenericBlock* inBlockPtr,
			std::vector<BlockData>& ioBlockDataVec,
			BlockPtrToIdUMap& ioBlockToIdxUMap,
			bool inTopFlag);

	bool fillBlockNets(
			const GenericInst* inParentInstPtr,
			const GenericBlock* inBlockPtr,
			BlockData& ioBlockData,
			StringToIdMap& outNetNameToIdxMap);

	bool fillBlockInsts(
			const GenericInst* inParentInstPtr,
			const GenericBlock* inBlockPtr,
			std::vector<BlockData>& ioBlockDataVec,
			BlockPtrToIdUMap& ioBlockToIdxUMap,
			const StringToIdMap& inNetNameToIdxMap,
			BlockData& ioBlockData);

};



/**
 * Adds ports info of the block/instance in the data.
 * Returns false if object pointer is null or failed to fill connected nets.
 * Can ignore bit-ports if it's necessary.
 */
template<typename _ObjectType, typename _DataType, typename _PortType>
bool AbsNetlistProcessorBase::fillObjectPorts(
						_ObjectType* inObjectPtr,
						_DataType& ioData,
						const StringToIdMap& inNetNameToIdxMap,
						bool inIgnoreBits) {
	if(!inObjectPtr)
		return false;

	//adding info of all ports
	std::vector<_PortType*> portsVec;
	getPorts(inObjectPtr, portsVec);
	for(auto portPtr : portsVec) {
		if(!portPtr)
			continue;

		//ignoring bit-ports if it is necessary
		if(inIgnoreBits && isBit(portPtr))
			continue;

		PortData data;
		data.mName = getName(portPtr);
		data.mInput = isInput(portPtr);
		data.mOutput = isOutput(portPtr);
		data.mBusFlag = isBus(portPtr);
		data.mRangeFrom = getBusRangeFrom(portPtr);
		data.mRangeTo = getBusRangeTo(portPtr);

		//filling net connectivity vector
		if(!fillNetsConnVec(portPtr,
				inNetNameToIdxMap,
				data.mConnNetIdxsVec))
			return false;

		ioData.mPortDataVec.push_back(data);
	}

	return true;
}


/**
 * Fills connected nets in vector for single/bus port/pin.
 * Returns false for nullptr object.
 */
template<typename _ObjectType>
bool AbsNetlistProcessorBase::fillNetsConnVec(
						_ObjectType* inObjectPtr,
						const StringToIdMap& inNetNameToIdxMap,
						std::vector<uint32_t>& outNetIdxVec) {
	if(!inObjectPtr)
		return false;

	uint32_t netIdx = 0;

	//for single port/pin vector may be empty or have one net idx
	if(!isBus(inObjectPtr)) {
		//leaving vector as is if there're no connections
		if(!getConnectedNetIdx(inObjectPtr, inNetNameToIdxMap, netIdx))
			return true;

		outNetIdxVec.push_back(netIdx);
		return true;
	}

	//for busses need to keep sizes
	uint32_t fromIdx = getBusRangeFrom(inObjectPtr);
	uint32_t toIdx = getBusRangeTo(inObjectPtr);
	_ObjectType* bitObjPtr = nullptr;

	int incr = 1;
	if(fromIdx > toIdx)
		incr = -1;

	//first sweeping all of them except object[toIdx]
	//for unconnected adding max(uint32_t)
	for(;fromIdx != toIdx; fromIdx += incr) {
		bitObjPtr = getBit(inObjectPtr, fromIdx);
		if(!getConnectedNetIdx(bitObjPtr, inNetNameToIdxMap, netIdx))
			outNetIdxVec.push_back(std::numeric_limits<uint32_t>::max());

		outNetIdxVec.push_back(netIdx);
	}

	bitObjPtr = getBit(inObjectPtr, toIdx);
	if(!getConnectedNetIdx(bitObjPtr, inNetNameToIdxMap, netIdx))
		outNetIdxVec.push_back(std::numeric_limits<uint32_t>::max());

	outNetIdxVec.push_back(netIdx);

	return true;
}

/**
 * Finds net connected to pin/port.
 * Returns false if object is unconnected or is null.
 * Also returns false if net name wasn't registered.
 */
template<typename _ObjectType>
bool AbsNetlistProcessorBase::getConnectedNetIdx(
						_ObjectType* inObjectPtr,
						const StringToIdMap& inNetNameToIdxMap,
						uint32_t& outNetIdx) {
	if(!inObjectPtr)
		return false;

	GenericNet* netPtr = getConnectedNet(inObjectPtr);
	if(!netPtr)
		return false;

	auto nameIt = inNetNameToIdxMap.find(getName(netPtr));
	//unregistered net is a fail!
	if(nameIt == inNetNameToIdxMap.end())
		return false;

	outNetIdx = nameIt->second;
	return true;
}




}


#endif /* SRC_CLIENT_ABSNETLISTPROCESSORBASE_HPP_ */
