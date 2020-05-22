#pragma once

/* This file contains definitions for a whole spec implementation (eg. ICD and Loader) */
#ifndef _HK_SPEC_DEF_SUFFIX
#define _HK_SPEC_DEF_SUFFIX
#endif

#define _HK_FN2(fn, suf) fn##suf
#define _HK_FN(fn, suf) _HK_FN2(fn, suf)

/* Instance */
PFN_hkVoidFunction _HK_FN(hkGetInstanceProcAddr, _HK_SPEC_DEF_SUFFIX)(
	HkInstance instance, const char* pname);
HkResult _HK_FN(hkEnumerateInstanceVersion, _HK_SPEC_DEF_SUFFIX)(
	uint32_t* pApiVersion);
HkResult _HK_FN(hkEnumerateInstanceExtensionProperties, _HK_SPEC_DEF_SUFFIX)(
	uint32_t* pPropertyCount,
	HkExtensionProperties* pProperties);
HkResult _HK_FN(hkEnumerateInstanceLayerProperties, _HK_SPEC_DEF_SUFFIX)(
	uint32_t* pPropertyCount,
	HkLayerProperties* pProperties);
HkResult _HK_FN(hkCreateInstance, _HK_SPEC_DEF_SUFFIX)(
	const HkInstanceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkInstance* pInstance);

/*Instance functions*/
void _HK_FN(hkDestroyInstance, _HK_SPEC_DEF_SUFFIX)(HkInstance instance);
PFN_hkVoidFunction _HK_FN(hkGetDeviceProcAddr, _HK_SPEC_DEF_SUFFIX)(
	HkDevice device,
	const char* pName);

/*--Physical Device */
HkResult _HK_FN(hkEnumeratePhysicalDevices, _HK_SPEC_DEF_SUFFIX)(
	HkInstance instance,
	uint32_t* pPhysicalDeviceCount,
	HkPhysicalDevice* pPhysicalDevices);
void _HK_FN(hkGetPhysicalDeviceProperties, _HK_SPEC_DEF_SUFFIX)(
	HkPhysicalDevice physicalDevice,
	HkPhysicalDeviceProperties* pProperties);
void _HK_FN(hkGetPhysicalDeviceQueueFamilyProperties, _HK_SPEC_DEF_SUFFIX)(
	HkPhysicalDevice physicalDevice,
	uint32_t* pQueueFamilyPropertyCount,
	HkQueueFamilyProperties* pQueueFamilyProperties);
HkResult _HK_FN(hkCreateDevice, _HK_SPEC_DEF_SUFFIX)(
	HkPhysicalDevice physicalDevice,
	const HkDeviceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkDevice* pDevice);

#undef _HK_FN
#undef _HK_FN2