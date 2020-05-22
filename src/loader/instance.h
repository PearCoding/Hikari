#pragma once

#include "hikari.h"
#include "hk_icd.h"
#include "hk_layer.h"

typedef struct _HkLayer {
	void* pHandle;
	HkNegotiateLayerInterface interface;
} _HkLayer;

typedef struct _HkICD {
	void* pHandle;
	HkNegotiateICDInterface interface;
} _HkICD;

typedef struct HkInstanceObj {
	HkAllocationCallbacks alloc;
	uint32_t layerCount;
	_HkLayer* pLayers;
	uint32_t icdCount;
	_HkICD* pICDs;
} HkInstanceObj;

inline HkInstanceObj* get_instance(HkInstance instance) {
	return (HkInstanceObj*)instance;
}

/* Utility functions */
inline void* _hk_alloc(HkInstance instance,
					   size_t size, size_t alignment = 1)
{
	HkInstanceObj* inst = get_instance(instance);
	return inst->alloc.pAllocate(inst->alloc.pUserData, size, alignment);
}

inline void* _hk_realloc(HkInstance instance,
						 void* pOriginal, size_t size, size_t alignment = 1)
{
	HkInstanceObj* inst = get_instance(instance);
	return inst->alloc.pReallocate(inst->alloc.pUserData, pOriginal, size, alignment);
}

inline void _hk_free(HkInstance instance,
					 void* pData, size_t size)
{
	HkInstanceObj* inst = get_instance(instance);
	inst->alloc.pFree(inst->alloc.pUserData, pData, size);
}
