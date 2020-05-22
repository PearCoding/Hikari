#include "hikari.h"
#include "hk_layer.h"
#include "layer.h"

extern PFN_hkVoidFunction hkGetInstanceProcAddr_layer(HkInstance instance, const char* pname);
HK_EXPORT HkResult hkNegotiateLoaderLayerInterface(HkNegotiateLayerInterface* pVersionStruct)
{
	pVersionStruct->sType						= HK_LAYER_NEGOTIATE_INTERFACE_STRUCT;
	pVersionStruct->pNext						= NULL;
	pVersionStruct->loaderLayerInterfaceVersion = 1;
	pVersionStruct->pfnGetInstanceProcAddr		= hkGetInstanceProcAddr_layer;
	pVersionStruct->pfnGetDeviceProcAddr		= NULL;

	return HK_SUCCESS;
}