#pragma once

#include "hk_layer.h"

HkResult _hk_setup_layer(HkInstance instance,
						 const HkInstanceCreateInfo* pCreateInfo);

void _hk_create_instance_dispatch_chain(HkInstance instance,
										PFN_hkGetInstanceProcAddr fTerminator,
										HkLayerInstanceCreateInfo*& pLayerCreateInfo);
void _hk_create_device_dispatch_chain(HkInstance instance,
									  PFN_hkGetDeviceProcAddr fTerminator,
									  HkLayerInstanceCreateInfo*& pLayerCreateInfo);
void _hk_destroy_dispatch_chain(HkInstance instance,
								HkLayerInstanceCreateInfo* pLayerCreateInfo);
void _hk_destroy_layer(HkInstance instance);