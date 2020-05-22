#include "hikari.h"
#include "hk_layer.h"
#include "layer.h"

#include <unordered_map>

struct LayerDispatchTable {
	PFN_hkGetInstanceProcAddr pNextGetInstanceProcAddr;
};
std::unordered_map<HkInstance, LayerDispatchTable> sDispatchTable;
PFN_hkVoidFunction hkNextInstanceProcAddr(HkInstance instance, const char* pname)
{
	return sDispatchTable[instance].pNextGetInstanceProcAddr(instance, pname);
}

HkResult hkCreateInstance_layer(
	const HkInstanceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkInstance* pInstance)
{
	HkLayerInstanceCreateInfo* layerCreateInfo = (HkLayerInstanceCreateInfo*)pCreateInfo->pNext;

	// step through the chain of pNext until we get to the link info
	while (layerCreateInfo && (layerCreateInfo->sType != HK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO || layerCreateInfo->function != HK_LAYER_LINK_INFO)) {
		layerCreateInfo = (HkLayerInstanceCreateInfo*)layerCreateInfo->pNext;
	}

	if (layerCreateInfo == HK_NULL_HANDLE) {
		// No loader instance create info
		return HK_ERROR_INITIALIZATION_FAILED;
	}

	PFN_hkGetInstanceProcAddr gpa = layerCreateInfo->u.pLayerInfo->pfnNextGetInstanceProcAddr;
	// move chain on for next layer
	layerCreateInfo->u.pLayerInfo = layerCreateInfo->u.pLayerInfo->pNext;

	PFN_hkCreateInstance createFunc = (PFN_hkCreateInstance)gpa(HK_NULL_HANDLE, "hkCreateInstance");

	if (createFunc == HK_NULL_HANDLE) {
		// No loader instance create info
		return HK_ERROR_INITIALIZATION_FAILED;
	}

	HkResult ret = createFunc(pCreateInfo, pAllocCalls, pInstance);

	LayerDispatchTable dispatchTable;
	dispatchTable.pNextGetInstanceProcAddr = gpa;
	sDispatchTable[*pInstance]			   = dispatchTable;

	return ret;
}

void hkDestroyInstance_layer(HkInstance instance)
{
	if (instance == HK_NULL_HANDLE) {
		_hk_p_error(_HK_LAYER_STR, "Invalid instance given!\n");
	} else {
		((PFN_hkDestroyInstance)hkNextInstanceProcAddr(instance, "hkDestroyInstance"))(instance);
	}
}