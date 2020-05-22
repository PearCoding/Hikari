#include "layer.h"
#include "hk_layer.h"
#include "instance.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <dlfcn.h>

#define _HK_LAYER_ENV_NAME "HK_LAYER"
#define _HK_LAYER_ENV_PATH_NAME "HK_LAYER_PATH"
#define _HK_LAYER_LIB_PREFIX "libHK_LAYER_"
#define _HK_ENV_DELIMITER ':'

HkResult hkEnumerateInstanceLayerProperties_loader(
	uint32_t* pPropertyCount,
	HkLayerProperties* pProperties)
{
	if (pPropertyCount != HK_NULL_HANDLE)
		*pPropertyCount = 1;

	/* TODO */
	return HK_SUCCESS;
}

static HkResult loadLayer(size_t i,
						  const std::string& layerPath, HkInstance instance)
{
	void* dll = dlopen(layerPath.c_str(), RTLD_NOW | RTLD_LOCAL);
	if (dll == HK_NULL_HANDLE) {
		std::cerr << dlerror() << std::endl;
		return HK_ERROR_INITIALIZATION_FAILED;
	}
	dlerror();

	PFN_hkNegotiateLoaderLayerInterface initFunc
		= reinterpret_cast<PFN_hkNegotiateLoaderLayerInterface>(dlsym(dll, HK_LAYER_DEFAULT_NEGOTIATE_FUNC_NAME));
	const char* err = dlerror();
	if (initFunc == HK_NULL_HANDLE || err != HK_NULL_HANDLE) {
		std::cerr << err << std::endl;
		dlclose(dll);
		return HK_ERROR_INITIALIZATION_FAILED;
	}

	// Setup
	HkNegotiateLayerInterface versionStruct;
	HkResult result = initFunc(&versionStruct);
	if (result != HK_SUCCESS) {
		dlclose(dll);
		return result;
	}

	if (versionStruct.sType != HK_LAYER_NEGOTIATE_INTERFACE_STRUCT
		|| versionStruct.pfnGetInstanceProcAddr == HK_NULL_HANDLE) {
		dlclose(dll);
		return HK_ERROR_INITIALIZATION_FAILED;
	}

	// Save information
	HkInstanceObj* inst		 = get_instance(instance);
	inst->pLayers[i].pHandle = dll;
	memcpy(&inst->pLayers[i].interface, &versionStruct, sizeof(versionStruct));

	return HK_SUCCESS;
}

static std::unordered_map<std::string, std::string>
getLayersFromPath(const std::string& path,
				  const std::vector<std::string>& requestedPlugins)
{
	std::unordered_map<std::string, std::string> layers;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (!entry.is_regular_file())
			continue;
		if (entry.path().extension() == ".so"
			&& boost::starts_with(entry.path().stem().string(), _HK_LAYER_LIB_PREFIX)) {
			std::string plugin = boost::erase_first_copy(entry.path().stem().string(), "lib");
			std::cout << entry.path() << std::endl;
			if (std::find(requestedPlugins.begin(), requestedPlugins.end(), plugin) != requestedPlugins.end()) {
				if (layers.count(plugin)) {
					std::cerr << "HIKARI_ERROR: Multiple layers with same identifier!" << std::endl;
				}

				layers[plugin] = entry.path().string();
			}
		}
	}

	return layers;
}

extern PFN_hkVoidFunction hkGetInstanceProcAddr_terminator(HkInstance instance, const char* pname);
HkResult _hk_setup_layer(HkInstance instance,
						 const HkInstanceCreateInfo* pCreateInfo)
{
	HkInstanceObj* inst = get_instance(instance);
	std::vector<std::string> paths;
	std::vector<std::string> plugins;

	/* Order:
	 1. Implicit layers (NOT IMPLEMENTED)
	 2. Env-enabled layers
	 3. App-enabled layers
	 */

	const char* envPaths = std::getenv(_HK_LAYER_ENV_PATH_NAME);
	if (envPaths)
		boost::split(paths, envPaths, [](char c) { return c == _HK_ENV_DELIMITER; });

	const char* envLayers = std::getenv(_HK_LAYER_ENV_NAME);
	if (envLayers)
		boost::split(plugins, envLayers, [](char c) { return c == _HK_ENV_DELIMITER; });

	for (size_t i = 0; i < pCreateInfo->enabledLayerCount; ++i) {
		const char* plugin = pCreateInfo->ppEnabledLayerNames[i];
		if (std::find(plugins.begin(), plugins.end(), plugin) == plugins.end())
			plugins.push_back(plugin);
	}

	std::unordered_map<std::string, std::string> layers;
	for (const std::string& path : paths) {
		auto bin = getLayersFromPath(path.c_str(), plugins);
		layers.merge(bin);
	}

	inst->layerCount = layers.size();
	if (layers.empty()) {
		inst->pLayers = HK_NULL_HANDLE;
		return HK_SUCCESS;
	}

	inst->pLayers = (_HkLayer*)_hk_alloc(instance, inst->layerCount * sizeof(_HkLayer));

	size_t i = 0;
	for (const auto& plugin : layers) {
		std::cout << "Loading Layer " << plugin.first << std::endl;
		HkResult result = loadLayer(i, plugin.second, instance);
		if (result != HK_SUCCESS)
			return result;
		++i;
	}

	return HK_SUCCESS;
}

void _hk_create_instance_dispatch_chain(HkInstance instance,
										PFN_hkGetInstanceProcAddr fTerminator,
										HkLayerInstanceCreateInfo*& pLayerCreateInfo)
{
	HkInstanceObj* inst = get_instance(instance);

	pLayerCreateInfo = (HkLayerInstanceCreateInfo*)_hk_alloc(instance, sizeof(HkLayerInstanceCreateInfo) * (inst->layerCount + 1));

	HkLayerInstanceLink* prevLinkInfo = HK_NULL_HANDLE;

	for (size_t i = 0; i < inst->layerCount; ++i) {
		pLayerCreateInfo[i].sType = HK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
		pLayerCreateInfo[i].pNext = &pLayerCreateInfo[i + 1];

		pLayerCreateInfo[i].function								 = HK_LAYER_LINK_INFO;
		pLayerCreateInfo[i].u.pLayerInfo							 = (HkLayerInstanceLink*)_hk_alloc(instance, sizeof(HkLayerInstanceLink));
		pLayerCreateInfo[i].u.pLayerInfo->pNext						 = HK_NULL_HANDLE;
		pLayerCreateInfo[i].u.pLayerInfo->pfnNextGetInstanceProcAddr = HK_NULL_HANDLE;

		if (prevLinkInfo) {
			prevLinkInfo->pNext						 = pLayerCreateInfo[i].u.pLayerInfo;
			prevLinkInfo->pfnNextGetInstanceProcAddr = inst->pLayers[i].interface.pfnGetInstanceProcAddr;
		}

		prevLinkInfo = pLayerCreateInfo[i].u.pLayerInfo;
	}

	// Add terminator code
	pLayerCreateInfo[inst->layerCount].sType = HK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
	pLayerCreateInfo[inst->layerCount].pNext = HK_NULL_HANDLE;

	pLayerCreateInfo[inst->layerCount].function									= HK_LAYER_LINK_INFO;
	pLayerCreateInfo[inst->layerCount].u.pLayerInfo								= (HkLayerInstanceLink*)_hk_alloc(instance, sizeof(HkLayerInstanceLink));
	pLayerCreateInfo[inst->layerCount].u.pLayerInfo->pNext						= HK_NULL_HANDLE;
	pLayerCreateInfo[inst->layerCount].u.pLayerInfo->pfnNextGetInstanceProcAddr = HK_NULL_HANDLE;

	if (prevLinkInfo) {
		prevLinkInfo->pNext						 = pLayerCreateInfo[inst->layerCount].u.pLayerInfo;
		prevLinkInfo->pfnNextGetInstanceProcAddr = fTerminator;
	}
}

void _hk_create_device_dispatch_chain(HkInstance instance,
									  PFN_hkGetDeviceProcAddr fTerminator,
									  HkLayerInstanceCreateInfo*& pLayerCreateInfo)
{
	HkInstanceObj* inst = get_instance(instance);

	// Check device capable layers
	uint32_t layer_count = 0;
	for (size_t i = 0; i < inst->layerCount; ++i) {
		if (inst->pLayers[i].interface.pfnGetDeviceProcAddr != HK_NULL_HANDLE)
			++layer_count;
	}

	if(layer_count == 0) {
		pLayerCreateInfo = HK_NULL_HANDLE;
		return;
	}

	pLayerCreateInfo = (HkLayerInstanceCreateInfo*)_hk_alloc(instance, sizeof(HkLayerInstanceCreateInfo) * (layer_count + 1));

	HkLayerInstanceLink* prevLinkInfo = HK_NULL_HANDLE;

	size_t i = 0;
	for (size_t k = 0; k < inst->layerCount; ++k) {
		if (inst->pLayers[k].interface.pfnGetDeviceProcAddr == HK_NULL_HANDLE)
			continue;

		pLayerCreateInfo[i].sType = HK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
		pLayerCreateInfo[i].pNext = &pLayerCreateInfo[i + 1];

		pLayerCreateInfo[i].function								 = HK_LAYER_LINK_INFO;
		pLayerCreateInfo[i].u.pLayerInfo							 = (HkLayerInstanceLink*)_hk_alloc(instance, sizeof(HkLayerInstanceLink));
		pLayerCreateInfo[i].u.pLayerInfo->pNext						 = HK_NULL_HANDLE;
		pLayerCreateInfo[i].u.pLayerInfo->pfnNextGetInstanceProcAddr = HK_NULL_HANDLE;
		pLayerCreateInfo[i].u.pLayerInfo->pfnNextGetDeviceProcAddr   = HK_NULL_HANDLE;

		if (prevLinkInfo) {
			prevLinkInfo->pNext						 = pLayerCreateInfo[i].u.pLayerInfo;
			prevLinkInfo->pfnNextGetInstanceProcAddr = inst->pLayers[k].interface.pfnGetInstanceProcAddr;
			prevLinkInfo->pfnNextGetDeviceProcAddr   = inst->pLayers[k].interface.pfnGetDeviceProcAddr;
		}

		prevLinkInfo = pLayerCreateInfo[i].u.pLayerInfo;
		++i;
	}

	// Add terminator code
	pLayerCreateInfo[layer_count].sType		 = HK_STRUCTURE_TYPE_LOADER_INSTANCE_CREATE_INFO;
	pLayerCreateInfo[inst->layerCount].pNext = HK_NULL_HANDLE;

	pLayerCreateInfo[inst->layerCount].function								  = HK_LAYER_LINK_INFO;
	pLayerCreateInfo[inst->layerCount].u.pLayerInfo							  = (HkLayerInstanceLink*)_hk_alloc(instance, sizeof(HkLayerInstanceLink));
	pLayerCreateInfo[inst->layerCount].u.pLayerInfo->pNext					  = HK_NULL_HANDLE;
	pLayerCreateInfo[inst->layerCount].u.pLayerInfo->pfnNextGetDeviceProcAddr = HK_NULL_HANDLE;

	if (prevLinkInfo) {
		prevLinkInfo->pNext					   = pLayerCreateInfo[inst->layerCount].u.pLayerInfo;
		prevLinkInfo->pfnNextGetDeviceProcAddr = fTerminator;
	}
}

void _hk_destroy_dispatch_chain(HkInstance instance,
								HkLayerInstanceCreateInfo* pLayerCreateInfo)
{
	size_t count = 0;
	for (HkLayerInstanceCreateInfo* info = pLayerCreateInfo;
		 info;
		 info = (HkLayerInstanceCreateInfo*)info->pNext) {
		++count;
		if (info->function == HK_LAYER_LINK_INFO) {
			_hk_free(instance, info->u.pLayerInfo, sizeof(info->u.pLayerInfo));
		}
	}

	_hk_free(instance, pLayerCreateInfo, count * sizeof(HkLayerInstanceCreateInfo));
}

void _hk_destroy_layer(HkInstance instance)
{
	HkInstanceObj* inst = get_instance(instance);
	if (inst->layerCount == 0)
		return;

	for (size_t i = 0; i < inst->layerCount; ++i) {
		dlclose(inst->pLayers[i].pHandle);
	}

	_hk_free(instance, inst->pLayers, sizeof(_HkLayer) * inst->layerCount);
}