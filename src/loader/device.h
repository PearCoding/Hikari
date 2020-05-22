#pragma once

#include "hikari.h"

HkResult hkCreateDevice(
	HkPhysicalDevice physicalDevice,
	const HkDeviceCreateInfo* pCreateInfo,
	const HkAllocationCallbacks* pAllocCalls,
	HkDevice* pDevice);