#include "instance.h"
#include "icd.h"
#include "layer.h"
#include "terminator.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void* _hk_std_alloc(void* pUserData, size_t size, size_t alignment)
{
	return malloc(size);
}

void* _hk_std_realloc(void* pUserData, void* pOriginal, size_t size, size_t alignment)
{
	return realloc(pOriginal, size);
}

void _hk_std_free(void* pUserData, void* pData, size_t size)
{
	free(pData);
}

HkResult hkCreateInstance_loader(
	const HkInstanceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkInstance* pInstance)
{
	HkResult result;
	PFN_hkAllocationFunction _allocate = (pAllocCalls ? pAllocCalls->pAllocate : _hk_std_alloc);
	void* pUserData					   = (pAllocCalls ? pAllocCalls->pUserData : NULL);

	HkInstanceObj* inst = (HkInstanceObj*)_allocate(pUserData, sizeof(HkInstanceObj), 1);
	if (inst == NULL)
		return HK_ERROR_OUT_OF_HOST_MEMORY;

	if (pAllocCalls) {
		memcpy(&inst->alloc, pAllocCalls, sizeof(HkAllocationCallbacks));
	} else {
		inst->alloc.pUserData   = NULL;
		inst->alloc.pAllocate   = _hk_std_alloc;
		inst->alloc.pReallocate = _hk_std_realloc;
		inst->alloc.pFree		= _hk_std_free;
	}

	*pInstance = (HkInstance)inst;

	// Load ICDs
	result = _hk_setup_icd(*pInstance, pCreateInfo);
	if (result != HK_SUCCESS)
		return result;

	// Load layers
	result = _hk_setup_layer(*pInstance, pCreateInfo);
	if (result != HK_SUCCESS) {
		_hk_destroy_icd(*pInstance);
		return result;
	}

	// Setup the chain
	if (inst->layerCount > 0) {
		HkLayerInstanceCreateInfo* pLayerCreateInfo = HK_NULL_HANDLE;
		_hk_create_instance_dispatch_chain(*pInstance,
										   hkGetInstanceProcAddr_terminator,
										   pLayerCreateInfo);
		HkInstanceCreateInfo createInfo2 = *pCreateInfo;
		createInfo2.pNext				 = pLayerCreateInfo;

		assert(pLayerCreateInfo->function == HK_LAYER_LINK_INFO);

		PFN_hkCreateInstance nextCreateInstance = (PFN_hkCreateInstance)inst->pLayers[0].interface.pfnGetInstanceProcAddr(HK_NULL_HANDLE, "hkCreateInstance");

		result = nextCreateInstance(&createInfo2, pAllocCalls, pInstance);

		_hk_destroy_dispatch_chain(*pInstance, pLayerCreateInfo);
	} else {
		result = hkCreateInstance_terminator(pCreateInfo, pAllocCalls, pInstance);
	}

	return result;
}

void hkDestroyInstance_loader(HkInstance instance)
{
	PFN_hkFreeFunction _free;
	void* pUserData;

	if (instance == NULL)
		return;

	HkInstanceObj* inst = get_instance(instance);
	if (inst->layerCount > 0) {
		PFN_hkDestroyInstance pfn = (PFN_hkDestroyInstance)inst->pLayers[0].interface.pfnGetInstanceProcAddr(instance, "hkDestroyInstance");
		pfn(instance);
	} else {
		hkDestroyInstance_terminator(instance);
	}

	_hk_destroy_layer(instance);
	_hk_destroy_icd(instance);

	_free	 = inst->alloc.pFree;
	pUserData = inst->alloc.pUserData;

	_free(pUserData, instance, sizeof(HkInstanceObj));
}

// Name of function has to be exact like that
PFN_hkVoidFunction hkGetInstanceProcAddr(HkInstance instance, const char* pname)
{
	HkInstanceObj* inst = get_instance(instance);
	if (strcmp(pname, "hkCreateInstance") == 0) {
		return (PFN_hkVoidFunction)hkCreateInstance_loader;
	} else if (strcmp(pname, "hkDestroyInstance") == 0) {
		return (PFN_hkVoidFunction)hkDestroyInstance_loader;
	} else if (strcmp(pname, "hkGetInstanceProcAddr") == 0) {
		return (PFN_hkVoidFunction)hkGetInstanceProcAddr;
	} else if (instance && inst->layerCount > 0) {
		return inst->pLayers[0].interface.pfnGetInstanceProcAddr(instance, pname);
	} else { // Directly call terminator (No layers needed)
		return hkGetInstanceProcAddr_terminator(instance, pname);
	}

	return HK_NULL_HANDLE;
}