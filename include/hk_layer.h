#ifndef _HK_LAYER_H_
#define _HK_LAYER_H_ 1

#include "hikari.h"

typedef enum HkNegotiateLayerStructType {
	HK_LAYER_NEGOTIATE_UNINTIALIZED		= 0,
	HK_LAYER_NEGOTIATE_INTERFACE_STRUCT = 1,
} HkNegotiateLayerStructType;

typedef struct HkNegotiateLayerInterface {
	HkNegotiateLayerStructType sType;
	void* pNext;
	uint32_t loaderLayerInterfaceVersion;
	PFN_hkGetInstanceProcAddr pfnGetInstanceProcAddr;
	PFN_hkGetDeviceProcAddr pfnGetDeviceProcAddr;
} HkNegotiateLayerInterface;

/* Layer Initialization */
typedef enum HkLayerFunction {
	HK_LAYER_LINK_INFO = 0,
} HkLayerFunction;

typedef struct HkLayerInstanceLink {
	struct HkLayerInstanceLink* pNext;
	PFN_hkGetInstanceProcAddr pfnNextGetInstanceProcAddr;
	PFN_hkGetDeviceProcAddr pfnNextGetDeviceProcAddr;
} HkLayerInstanceLink;

typedef struct HkLayerInstanceCreateInfo {
	HkStructureType sType; // HK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO
	const void* pNext;
	HkLayerFunction function;
	union {
		HkLayerInstanceLink* pLayerInfo;
	} u;
} HkLayerInstanceCreateInfo;

#ifdef __cplusplus
extern "C" {
#endif

#define HK_LAYER_DEFAULT_NEGOTIATE_FUNC_NAME "hkNegotiateLoaderLayerInterface"
typedef HkResult(HKAPIENTRY* PFN_hkNegotiateLoaderLayerInterface)(
	HkNegotiateLayerInterface* pVersionStruct);

#ifdef __cplusplus
}
#endif

#endif /*_HK_LAYER_H_*/