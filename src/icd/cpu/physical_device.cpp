#include "objects.h"

#include <string.h>

// TODO: Bad way!!!
static HkPhysicalDeviceObj sPhysicalDevice;
HkResult hkEnumeratePhysicalDevices_icd(
	HkInstance instance,
	uint32_t* pPhysicalDeviceCount,
	HkPhysicalDevice* pPhysicalDevices)
{
	if (pPhysicalDevices == HK_NULL_HANDLE) {
		*pPhysicalDeviceCount = 1;
	} else if (*pPhysicalDeviceCount > 0) {
		*pPhysicalDeviceCount = 1;
		pPhysicalDevices[0]   = (HkPhysicalDevice)&sPhysicalDevice;
	}
	return HK_SUCCESS;
}

void hkGetPhysicalDeviceProperties_icd(
	HkPhysicalDevice physicalDevice,
	HkPhysicalDeviceProperties* pProperties)
{
	memset(pProperties, 0, sizeof(HkPhysicalDeviceProperties));

	strcpy(pProperties->deviceName, "STD_CPU");
	pProperties->deviceType	= HK_PHYSICAL_DEVICE_TYPE_CPU;
	pProperties->apiVersion	= HK_API_VERSION_1_0;
	pProperties->driverVersion = HK_MAKE_VERSION(0, 1);
	pProperties->deviceID	  = 0;
	pProperties->vendorID	  = 0;
}

void hkGetPhysicalDeviceQueueFamilyProperties_icd(
	HkPhysicalDevice physicalDevice,
	uint32_t* pQueueFamilyPropertyCount,
	HkQueueFamilyProperties* pQueueFamilyProperties)
{
	if (pQueueFamilyProperties == HK_NULL_HANDLE) {
		*pQueueFamilyPropertyCount = 1;
	} else if (*pQueueFamilyPropertyCount > 0) {
		*pQueueFamilyPropertyCount			 = 1;
		pQueueFamilyProperties[0].queueCount = 1;
		pQueueFamilyProperties[0].queueFlags = HK_QUEUE_TRACING_BIT | HK_QUEUE_TRANSFER_BIT | HK_QUEUE_SHADING_BIT;
	}
}