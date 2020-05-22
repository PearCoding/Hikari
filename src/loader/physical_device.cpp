#include "physical_device.h"
#include "icd.h"
#include "instance.h"

HkResult hkEnumeratePhysicalDevices(
	HkInstance instance,
	uint32_t* pPhysicalDeviceCount,
	HkPhysicalDevice* pPhysicalDevices)
{
	HkInstanceObj* inst = get_instance(instance);
	if (pPhysicalDevices == HK_NULL_HANDLE) {
		*pPhysicalDeviceCount = 0;
		uint32_t count		  = 0;

		_HK_EACH_ICD_START(inst, "hkEnumeratePhysicalDevices")
		HkResult result = ((PFN_hkEnumeratePhysicalDevices)(pfn))(instance, &count,
																  HK_NULL_HANDLE);
		if (result != HK_SUCCESS)
			return result;
		*pPhysicalDeviceCount += count;
		_HK_EACH_ICD_END

		return HK_SUCCESS;
	} else {
		uint32_t count   = 0;
		uint32_t written = 0;

		_HK_EACH_ICD_START(inst, "hkEnumeratePhysicalDevices")
		count = *pPhysicalDeviceCount - written;
		if (count == 0 || written > *pPhysicalDeviceCount)
			break;

		HkResult result = ((PFN_hkEnumeratePhysicalDevices)(pfn))(instance, &count,
																  &pPhysicalDevices[written]);

		if (result != HK_SUCCESS)
			return result;

		for (size_t i = written; i < written + count; ++i) {
			((HkICDObjectBase*)pPhysicalDevices[i])->instance = instance;
			((HkICDObjectBase*)pPhysicalDevices[i])->pHandle  = reinterpret_cast<void*>(icd);
		}
		written += count;

		_HK_EACH_ICD_END

		return HK_SUCCESS;
	}
}

void hkGetPhysicalDeviceProperties(
	HkPhysicalDevice physicalDevice,
	HkPhysicalDeviceProperties* pProperties)
{
	HkICDObjectBase* base = (HkICDObjectBase*)physicalDevice;
	_HkICD* icd			  = _hk_get_icd(base);

	PFN_hkGetPhysicalDeviceProperties proc = (PFN_hkGetPhysicalDeviceProperties)icd->interface.pfnGetInstanceProcAddr(base->instance, "hkGetPhysicalDeviceProperties");

	proc(physicalDevice, pProperties);
}

void hkGetPhysicalDeviceQueueFamilyProperties(
	HkPhysicalDevice physicalDevice,
	uint32_t* pQueueFamilyPropertyCount,
	HkQueueFamilyProperties* pQueueFamilyProperties)
{
	HkICDObjectBase* base = (HkICDObjectBase*)physicalDevice;
	_HkICD* icd			  = _hk_get_icd(base);

	PFN_hkGetPhysicalDeviceQueueFamilyProperties proc = (PFN_hkGetPhysicalDeviceQueueFamilyProperties)icd->interface.pfnGetInstanceProcAddr(base->instance, "hkGetPhysicalDeviceQueueFamilyProperties");

	proc(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
}