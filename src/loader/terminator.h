#pragma once

#include "hikari.h"

PFN_hkVoidFunction hkGetInstanceProcAddr_terminator(HkInstance instance, const char* pname);
HkResult hkCreateInstance_terminator(
	const HkInstanceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkInstance* pInstance);
void hkDestroyInstance_terminator(HkInstance instance);