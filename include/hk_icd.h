#ifndef _HK_ICD_H_
#define _HK_ICD_H_ 1

#include "hikari.h"

typedef enum HkNegotiateICDStructType {
	HK_ICD_NEGOTIATE_UNINTIALIZED	 = 0,
	HK_ICD_NEGOTIATE_INTERFACE_STRUCT = 1,
} HkNegotiateICDStructType;

typedef struct HkNegotiateICDInterface {
	HkNegotiateICDStructType sType;
	void* pNext;
	uint32_t loaderICDInterfaceVersion;
	PFN_hkGetInstanceProcAddr pfnGetInstanceProcAddr;
	PFN_hkGetDeviceProcAddr pfnGetDeviceProcAddr;
} HkNegotiateICDInterface;

typedef struct HkICDObjectBase {
	HkInstance instance;
	void* pHandle;
} HkICDObjectBase;

#ifdef __cplusplus
extern "C" {
#endif

#define HK_ICD_DEFAULT_NEGOTIATE_FUNC_NAME "hkNegotiateLoaderICDInterface"
typedef HkResult(HKAPIENTRY* PFN_hkNegotiateLoaderICDInterface)(
	HkNegotiateICDInterface* pVersionStruct);

#ifdef __cplusplus
}
#endif

#endif /*_HK_LAYER_H_*/