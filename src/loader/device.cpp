#include "device.h"
#include "icd.h"
#include "instance.h"
#include "layer.h"

#include <assert.h>

HkResult hkCreateDevice_loader(
	HkPhysicalDevice physicalDevice,
	const HkDeviceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkDevice* pDevice)
{
	HkResult result		  = HK_ERROR_INITIALIZATION_FAILED;
	HkICDObjectBase* base = (HkICDObjectBase*)physicalDevice;
	_HkICD* icd			  = _hk_get_icd(base);
	HkInstanceObj* inst   = get_instance(base->instance);

	if (pAllocCalls == NULL) {
		pAllocCalls = &inst->alloc;
	}

	bool hasLayer = false;
	if (inst->layerCount > 0) {
		HkLayerInstanceCreateInfo* pLayerCreateInfo = HK_NULL_HANDLE;
		_hk_create_device_dispatch_chain(base->instance,
										 icd->interface.pfnGetDeviceProcAddr,
										 pLayerCreateInfo);
		if (pLayerCreateInfo != HK_NULL_HANDLE) {
			hasLayer = true;

			HkDeviceCreateInfo createInfo2 = *pCreateInfo;
			createInfo2.pNext			   = pLayerCreateInfo;

			assert(pLayerCreateInfo->function == HK_LAYER_LINK_INFO);

			PFN_hkCreateDevice nextCreateDevice = (PFN_hkCreateDevice)inst->pLayers[0].interface.pfnGetInstanceProcAddr(HK_NULL_HANDLE, "hkCreateDevice");

			result = nextCreateDevice(physicalDevice, &createInfo2, pAllocCalls, pDevice);

			_hk_destroy_dispatch_chain(base->instance, pLayerCreateInfo);
		}
	}

	if (!hasLayer) {
		PFN_hkCreateDevice proc = (PFN_hkCreateDevice)icd->interface.pfnGetInstanceProcAddr(base->instance, "hkCreateDevice");
		result					= proc(physicalDevice, pCreateInfo, pAllocCalls, pDevice);
	}

	if (result == HK_SUCCESS) {
		HkICDObjectBase* devbase = (HkICDObjectBase*)*pDevice;
		devbase->instance		 = base->instance;
		devbase->pHandle		 = base->pHandle;
	}
	return result;
}