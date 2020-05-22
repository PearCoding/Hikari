#include "terminator.h"
#include "icd.h"
#include "instance.h"
#include "physical_device.h"

#include "utils/dispatch_table_helper.h"

#include <string.h>

HkResult hkCreateInstance_terminator(
	const HkInstanceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkInstance* pInstance)
{
	HkInstanceObj* inst = get_instance(*pInstance);
	_HK_EACH_ICD_START(inst, "hkCreateInstance")
	HkResult result = ((PFN_hkCreateInstance)pfn)(pCreateInfo, pAllocCalls, pInstance);
	if (result != HK_SUCCESS)
		return result;
	_HK_EACH_ICD_END

	return HK_SUCCESS;
}

HkResult hkEnumerateInstanceVersion_terminator(uint32_t* pApiVersion)
{
	if (pApiVersion != NULL)
		*pApiVersion = HK_API_VERSION_1_0;
	return HK_SUCCESS;
}

void hkDestroyInstance_terminator(HkInstance instance)
{
	HkInstanceObj* inst = get_instance(instance);
	_HK_EACH_ICD_START_NO_CHECK(inst, "hkDestroyInstance")
	if (pfn)
		((PFN_hkDestroyInstance)pfn)(instance);
	_HK_EACH_ICD_END
}

extern HkResult hkEnumerateInstanceExtensionProperties_loader(uint32_t* pPropertyCount, HkExtensionProperties* pProperties);
extern HkResult hkEnumerateInstanceLayerProperties_loader(uint32_t* pPropertyCount, HkLayerProperties* pProperties);
extern HkResult hkCreateDevice_loader(
	HkPhysicalDevice physicalDevice,
	const HkDeviceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkDevice* pDevice);

static _HkDispatchMap _hk_global_funcs[] = {
	_HK_DISPATCH_ENTRY_S(hkGetInstanceProcAddr, _terminator),
	_HK_DISPATCH_ENTRY_S(hkEnumerateInstanceVersion, _terminator),// LOADER?
	_HK_DISPATCH_ENTRY_S(hkEnumerateInstanceExtensionProperties, _loader),
	_HK_DISPATCH_ENTRY_S(hkEnumerateInstanceLayerProperties, _loader),
	_HK_DISPATCH_ENTRY_S(hkCreateInstance, _terminator),// LOADER??
	_HK_DISPATCH_ENTRY_END()
};

static _HkDispatchMap _hk_instance_funcs[] = {
	_HK_DISPATCH_ENTRY_S(hkDestroyInstance, _terminator),
	_HK_DISPATCH_ENTRY(hkEnumeratePhysicalDevices),
	_HK_DISPATCH_ENTRY(hkGetPhysicalDeviceProperties),
	_HK_DISPATCH_ENTRY(hkGetPhysicalDeviceQueueFamilyProperties),
	_HK_DISPATCH_ENTRY_S(hkCreateDevice, _loader),
	_HK_DISPATCH_ENTRY_END()
};

PFN_hkVoidFunction hkGetInstanceProcAddr_terminator(HkInstance instance, const char* pname)
{
	if (instance) {
		for (size_t i = 0; _hk_instance_funcs[i].name; ++i) {
			if (strcmp(pname, _hk_instance_funcs[i].name) == 0)
				return _hk_instance_funcs[i].func;
		}
	}

	for (size_t i = 0; _hk_global_funcs[i].name; ++i) {
		if (strcmp(pname, _hk_global_funcs[i].name) == 0)
			return _hk_global_funcs[i].func;
	}

	return HK_NULL_HANDLE;
}
