#pragma once

#include "hk_icd.h"
#include "instance.h"

HkResult _hk_setup_icd(HkInstance instance,
					   const HkInstanceCreateInfo* pCreateInfo);
void _hk_destroy_icd(HkInstance instance);

#define _HK_EACH_ICD_START_NO_CHECK(inst, func)           \
	for (size_t icd = 0; icd < (inst)->icdCount; ++icd) { \
		PFN_hkVoidFunction pfn = (inst)->pICDs[icd].interface.pfnGetInstanceProcAddr(HK_NULL_HANDLE, func);

#define _HK_EACH_ICD_START(inst, func)      \
	_HK_EACH_ICD_START_NO_CHECK(inst, func) \
	if (pfn == HK_NULL_HANDLE)              \
		return HK_ERROR_INCOMPATIBLE_DRIVER;

#define _HK_EACH_ICD_END }

inline static _HkICD* _hk_get_icd(HkICDObjectBase* base)
{
	return &get_instance(base->instance)->pICDs[reinterpret_cast<size_t>(base->pHandle)];
}