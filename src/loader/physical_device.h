#pragma once

#include "hikari.h"

HkResult hkEnumeratePhysicalDevices(
	HkInstance instance,
	uint32_t* pPhysicalDeviceCount,
	HkPhysicalDevice* pPhysicalDevices);

void hkGetPhysicalDeviceProperties(
	HkPhysicalDevice physicalDevice,
	HkPhysicalDeviceProperties* pProperties);

void hkGetPhysicalDeviceQueueFamilyProperties(
	HkPhysicalDevice physicalDevice,
	uint32_t* pQueueFamilyPropertyCount,
	HkQueueFamilyProperties* pQueueFamilyProperties);