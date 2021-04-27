#include "icd.h"
#include "instance.h"
#include "utils.h"

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <vector>

#include <dlfcn.h>

#define _HK_ICD_ENV_PATH_NAME "HK_ICD_PATH"
#define _HK_ICD_LIB_PREFIX "libHK_ICD_"

static HkResult loadICD(size_t i,
						const std::string& icdPath, HkInstance instance)
{
	void* dll = dlopen(icdPath.c_str(), RTLD_NOW | RTLD_LOCAL);
	if (dll == HK_NULL_HANDLE) {
		std::cerr << dlerror() << std::endl;
		return HK_ERROR_INITIALIZATION_FAILED;
	}
	dlerror();

	PFN_hkNegotiateLoaderICDInterface initFunc
		= reinterpret_cast<PFN_hkNegotiateLoaderICDInterface>(dlsym(dll, HK_ICD_DEFAULT_NEGOTIATE_FUNC_NAME));
	const char* err = dlerror();
	if (initFunc == HK_NULL_HANDLE || err != HK_NULL_HANDLE) {
		std::cerr << err << std::endl;
		dlclose(dll);
		return HK_ERROR_INITIALIZATION_FAILED;
	}

	// Setup
	HkNegotiateICDInterface versionStruct;
	HkResult result = initFunc(&versionStruct);
	if (result != HK_SUCCESS) {
		dlclose(dll);
		return result;
	}

	if (versionStruct.sType != HK_ICD_NEGOTIATE_INTERFACE_STRUCT
		|| versionStruct.pfnGetInstanceProcAddr == HK_NULL_HANDLE) {
		dlclose(dll);
		return HK_ERROR_INITIALIZATION_FAILED;
	}

	// Save information
	HkInstanceObj* inst	   = get_instance(instance);
	inst->pICDs[i].pHandle = dll;
	std::memcpy(&inst->pICDs[i].interface, &versionStruct, sizeof(versionStruct));

	return HK_SUCCESS;
}

static std::unordered_set<std::string> getICDsFromPath(const std::string& path)
{
	std::unordered_set<std::string> icds;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (!entry.is_regular_file())
			continue;
		if (entry.path().extension() == ".so"
			&& entry.path().stem().string().rfind(_HK_ICD_LIB_PREFIX, 0) == 0) {
			icds.emplace(entry.path().string());
			std::cout << entry.path() << std::endl;
		}
	}

	return icds;
}

HkResult _hk_setup_icd(HkInstance instance,
					   const HkInstanceCreateInfo* pCreateInfo)
{
	HkInstanceObj* inst = get_instance(instance);
	std::vector<std::string> paths;

	const char* envPaths = std::getenv(_HK_ICD_ENV_PATH_NAME);
	if (envPaths)
		hk_split_env(envPaths, paths);

	std::unordered_set<std::string> icds;
	for (const std::string& path : paths) {
		auto bin = getICDsFromPath(path.c_str());
		icds.merge(bin);
	}

	inst->icdCount = icds.size();
	if (icds.empty()) {
		inst->pICDs = HK_NULL_HANDLE;
		return HK_ERROR_INITIALIZATION_FAILED;
	}

	inst->pICDs = (_HkICD*)_hk_alloc(instance, inst->icdCount * sizeof(_HkICD));

	size_t i = 0;
	for (const auto& plugin : icds) {
		std::cout << "Loading ICD " << plugin << std::endl;
		HkResult result = loadICD(i, plugin, instance);
		if (result != HK_SUCCESS)
			return result;
		++i;
	}

	return HK_SUCCESS;
}

void _hk_destroy_icd(HkInstance instance)
{
	HkInstanceObj* inst = get_instance(instance);
	if (inst->icdCount == 0)
		return;

	for (size_t i = 0; i < inst->icdCount; ++i) {
		dlclose(inst->pICDs[i].pHandle);
	}

	_hk_free(instance, inst->pICDs, sizeof(_HkICD) * inst->icdCount);
}