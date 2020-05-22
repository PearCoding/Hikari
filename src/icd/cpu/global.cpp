#include "hikari.h"
#include "hk_icd.h"

extern PFN_hkVoidFunction hkGetInstanceProcAddr_icd(HkInstance instance, const char* pname);
HK_EXPORT HkResult hkNegotiateLoaderICDInterface(HkNegotiateICDInterface* pVersionStruct)
{
	pVersionStruct->sType					  = HK_ICD_NEGOTIATE_INTERFACE_STRUCT;
	pVersionStruct->pNext					  = NULL;
	pVersionStruct->loaderICDInterfaceVersion = 1;
	pVersionStruct->pfnGetInstanceProcAddr	= hkGetInstanceProcAddr_icd;

	return HK_SUCCESS;
}