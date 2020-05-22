#include "objects.h"

HkResult hkCreateDevice_icd(
	HkPhysicalDevice physicalDevice,
	const HkDeviceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkDevice* pDevice)
{
	HkDeviceObj* device = (HkDeviceObj*)pAllocCalls->pAllocate(pAllocCalls->pUserData, sizeof(HkDeviceObj), 1);

	if (device == NULL)
		return HK_ERROR_OUT_OF_HOST_MEMORY;

	device->alloc			= *pAllocCalls;
	device->pPhysicalDevice = get_physical_device(physicalDevice);

	*pDevice = (HkDevice)device;
	return HK_SUCCESS;
}

void hkDestroyDevice_icd(HkDevice device)
{
	HkDeviceObj* dev = get_device(device);
	auto _free		 = dev->alloc.pFree;
	auto pUserData   = dev->alloc.pUserData;

	_free(pUserData, device, sizeof(HkDeviceObj));
}