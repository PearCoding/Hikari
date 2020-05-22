#include "hikari.h"
#include "layer.h"

#include <string.h>

extern HkResult hkCreateInstance_layer(
	const HkInstanceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkInstance* pInstance);
extern void hkDestroyInstance_layer(HkInstance instance);

PFN_hkVoidFunction hkGetInstanceProcAddr_layer(HkInstance instance, const char* pname);
struct
{
	const char* name;
	PFN_hkVoidFunction func;
} _hk_layer_funcs[] = {
	{ "hkGetInstanceProcAddr", (PFN_hkVoidFunction)hkGetInstanceProcAddr_layer },
	{ "hkCreateInstance", (PFN_hkVoidFunction)hkCreateInstance_layer },
	{ "hkDestroyInstance", (PFN_hkVoidFunction)hkDestroyInstance_layer },
	{ NULL, NULL }
};

PFN_hkVoidFunction hkGetInstanceProcAddr_layer(HkInstance instance, const char* pname)
{
	for (size_t i = 0; _hk_layer_funcs[i].name; ++i) {
		if (strcmp(pname, _hk_layer_funcs[i].name) == 0)
			return _hk_layer_funcs[i].func;
	}

	return hkNextInstanceProcAddr(instance, pname);
}