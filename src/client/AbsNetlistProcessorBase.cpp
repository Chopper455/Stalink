#include "AbsNetlistProcessorBase.hpp"

#include <unordered_map>
#include <map>

namespace stamask {


/**
 * Empty constructor.
 */
AbsNetlistProcessorBase::AbsNetlistProcessorBase() {}


/**
 * Empty destructor.
 */
AbsNetlistProcessorBase::~AbsNetlistProcessorBase() {}


/**
 * Recursively fills block's content data in command's vector.
 */
bool AbsNetlistProcessorBase::fillBlockData(
						const GenericBlock* inBlockPtr,
						std::vector<BlockData>& ioBlockDataVec,
						BlockPtrToIdUMap& ioBlockToIdxUMap,
						bool inTopFlag) {
	if(!inBlockPtr)
		return false;

	//ignoring block if it was already registered
	auto blockNameIt = ioBlockToIdxUMap.find(inBlockPtr);
	if(blockNameIt != ioBlockToIdxUMap.end())
		return true;

	BlockData realData;

	uint32_t blockId = ioBlockDataVec.size();
	ioBlockToIdxUMap.emplace(inBlockPtr, blockId);
	//adding an initial dummy
	ioBlockDataVec.push_back(BlockData());

	realData.mName = getName(inBlockPtr);
	realData.mTopFlag = inTopFlag;
	realData.mLeafFlag = isLeafBlock(inBlockPtr);


	StringToIdMap netNameToIdxMap;
	//filling the nets, mapping their names to vector indexes
	if(!fillBlockNets(
		nullptr, inBlockPtr,
		realData, netNameToIdxMap))
	return false;

	//filling the ports (without bits, only single and busses)
	if(!fillObjectPorts<const GenericBlock, BlockData, GenericPort>(
			inBlockPtr,
			realData,
			netNameToIdxMap,
			true))
		return false;

	if(realData.mLeafFlag) {
		ioBlockDataVec[blockId] = realData;
		return true;
	}

	//filling the instances (without hierarchy, only the children on the block)
	if(!fillBlockInsts(
			nullptr, inBlockPtr,
			ioBlockDataVec,ioBlockToIdxUMap,
			netNameToIdxMap, realData))
		return false;

	ioBlockDataVec[blockId] = realData;
	return true;
}

/**
 * Fills net names in block data, maps them to indexes.
 */
bool AbsNetlistProcessorBase::fillBlockNets(
						const GenericInst* inParentInstPtr,
						const GenericBlock* inBlockPtr,
						BlockData& ioBlockData,
						StringToIdMap& outNetNameToIdxMap) {
	if(!inBlockPtr)
		return false;

	//getting nets and adding all net indexes
	std::vector<GenericNet*> netsVec;
	getBlockNets(inParentInstPtr, inBlockPtr, netsVec);

	uint32_t netId = 0;
	std::string netName;
	auto netIt = outNetNameToIdxMap.begin();
	for(GenericNet* netPtr : netsVec) {
		if(!netPtr)
			continue;

		if(isBus(netPtr))
			continue;

		netName = getName(netPtr);
		netIt = outNetNameToIdxMap.find(netName);
		if(netIt != outNetNameToIdxMap.end())
			continue;

		netId = ioBlockData.mNetNamesVec.size();
		ioBlockData.mNetNamesVec.push_back(netName);
		outNetNameToIdxMap.emplace(netName, netId);

		if(hasGndSource(netPtr))
			ioBlockData.mGndNetName = netName;
		if(hasVddSource(netPtr))
			ioBlockData.mVddNetName = netName;

	}

	return true;
}

/**
 * Adds data for all instance children of the block.
 * If master of the instance wasn't registered, then fills data for the master.
 * Returns false on null object,
 * failed master registration or failed ports data collection.
 */
bool AbsNetlistProcessorBase::fillBlockInsts(
						const GenericInst* inParentInstPtr,
						const GenericBlock* inBlockPtr,
						std::vector<BlockData>& ioBlockDataVec,
						BlockPtrToIdUMap& ioBlockToIdxUMap,
						const StringToIdMap& inNetNameToIdxMap,
						BlockData& ioBlockData) {
	if(!inBlockPtr)
		return false;

	//adding info of all ports
	std::vector<GenericInst*> instsVec;
	GenericBlock* masterPtr = nullptr;
	auto masterIdxIt = ioBlockToIdxUMap.begin();

	getBlockInsts(inParentInstPtr, inBlockPtr, instsVec);
	for(auto instPtr : instsVec) {
		if(!instPtr)
			continue;

		masterPtr = getMasterBlock(instPtr);
		masterIdxIt = ioBlockToIdxUMap.find(masterPtr);
		//have to dive into a unregistered block
		if(masterIdxIt == ioBlockToIdxUMap.end()) {
			fillBlockData(
					masterPtr, ioBlockDataVec,
					ioBlockToIdxUMap, false);
		}

		masterIdxIt = ioBlockToIdxUMap.find(masterPtr);
		//nothing will help if it didn't appear
		if(masterIdxIt == ioBlockToIdxUMap.end())
			return false;

		InstanceData data;
		data.mName = getName(instPtr);
		data.mMasterBlockIdx = masterIdxIt->second;

		if(!fillObjectPorts<GenericInst, InstanceData, GenericPin>(
				instPtr, data, inNetNameToIdxMap, true))
			return false;

		ioBlockData.mInstDataVec.push_back(data);
	}

	return true;
}


}



