#pragma once

#include "hikari.h"
#include "hk_icd.h"

typedef struct HkPhysicalDeviceObj {
	HkICDObjectBase base; // Has to be the first element!
} HkPhysicalDeviceObj;

inline HkPhysicalDeviceObj* get_physical_device(HkPhysicalDevice physical_device)
{
	return (HkPhysicalDeviceObj*)physical_device;
}

typedef struct HkDeviceObj {
	HkICDObjectBase base;
	HkAllocationCallbacks alloc;
	HkPhysicalDeviceObj* pPhysicalDevice;
} HkDeviceObj;

inline HkDeviceObj* get_device(HkDevice device)
{
	return (HkDeviceObj*)device;
}