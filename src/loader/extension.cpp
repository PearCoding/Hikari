#include "hikari.h"

HkResult hkEnumerateInstanceExtensionProperties_loader(
	uint32_t* pPropertyCount,
	HkExtensionProperties* pProperties)
{
	if (pPropertyCount != HK_NULL_HANDLE)
		*pPropertyCount = 0;

	return HK_SUCCESS;
}