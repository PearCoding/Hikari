#include "hikari.h"

#define _HK_SPEC_DEF_SUFFIX _icd
#include "utils/dispatch_table_helper.h"
#include "utils/spec_def.h"

#include <string.h>

_HkDispatchMap _hk_layer_funcs[] = {
	_HK_DISPATCH_ENTRY_S(hkGetInstanceProcAddr, _icd),
	_HK_DISPATCH_ENTRY_S(hkCreateInstance, _icd),
	_HK_DISPATCH_ENTRY_S(hkDestroyInstance, _icd),
	_HK_DISPATCH_ENTRY_S(hkEnumeratePhysicalDevices, _icd),
	_HK_DISPATCH_ENTRY_S(hkGetPhysicalDeviceProperties, _icd),
	_HK_DISPATCH_ENTRY_S(hkGetPhysicalDeviceQueueFamilyProperties, _icd),
	_HK_DISPATCH_ENTRY_S(hkCreateDevice, _icd),
	_HK_DISPATCH_ENTRY_END()
};

PFN_hkVoidFunction hkGetInstanceProcAddr_icd(HkInstance instance, const char* pname)
{
	for (size_t i = 0; _hk_layer_funcs[i].name; ++i) {
		if (strcmp(pname, _hk_layer_funcs[i].name) == 0)
			return _hk_layer_funcs[i].func;
	}

	return HK_NULL_HANDLE;
}