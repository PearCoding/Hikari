#include "hikari.h"

#include <iostream>
#include <vector>

#define str(a) #a
#define INSTANCE_PROC(inst, name) \
	PFN_##name name = (PFN_##name)hkGetInstanceProcAddr(inst, str(name))
#define DEVICE_PROC(dev, name) \
	PFN_##name name = (PFN_##name)hkGetDeviceProcAddr(dev, str(name))

HkInstance setup_instance()
{
	INSTANCE_PROC(HK_NULL_HANDLE, hkCreateInstance);
	INSTANCE_PROC(HK_NULL_HANDLE, hkEnumerateInstanceVersion);

	if (!hkCreateInstance || !hkEnumerateInstanceVersion)
		return HK_NULL_HANDLE;

	HkInstance instance;
	HkResult result;
	uint32_t version;

	result = hkEnumerateInstanceVersion(&version);
	if (result != HK_SUCCESS)
		return HK_NULL_HANDLE;

	/*const char* instance_validation_layers[] = {
		"HK_LAYER_STD_validation"
	};*/
	HkInstanceCreateInfo createInfo = {
		HK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		NULL,
		NULL,
		0,
		NULL,
		0,
		NULL
	};

	std::cout << "API Version: " << HK_VERSION_MAJOR(version) << "." << HK_VERSION_MINOR(version)
			  << std::endl;

	result = hkCreateInstance(&createInfo, NULL, &instance);

	return (result == HK_SUCCESS) ? instance : HK_NULL_HANDLE;
}

void destroy_instance(HkInstance instance)
{
	INSTANCE_PROC(instance, hkDestroyInstance);

	if (hkDestroyInstance)
		hkDestroyInstance(instance);
}

HkPhysicalDevice pick_physical_device(HkInstance instance)
{
	INSTANCE_PROC(instance, hkEnumeratePhysicalDevices);
	INSTANCE_PROC(instance, hkGetPhysicalDeviceProperties);

	if (!hkEnumeratePhysicalDevices || !hkGetPhysicalDeviceProperties)
		return HK_NULL_HANDLE;

	uint32_t count = 0;
	if (hkEnumeratePhysicalDevices(instance, &count, HK_NULL_HANDLE) != HK_SUCCESS)
		return HK_NULL_HANDLE;

	std::vector<HkPhysicalDevice> devices;
	devices.resize(count);
	if (hkEnumeratePhysicalDevices(instance, &count, devices.data()) != HK_SUCCESS)
		return HK_NULL_HANDLE;

	std::cout << count << " physical devices found." << std::endl;

	for (size_t i = 0; i < count; ++i) {
		HkPhysicalDeviceProperties prop;
		hkGetPhysicalDeviceProperties(devices[i], &prop);

		std::cout << "PD[" << i << "]" << std::endl
				  << "  Name:   " << prop.deviceName << std::endl
				  << "  API:    " << HK_VERSION_MAJOR(prop.apiVersion) << "." << HK_VERSION_MINOR(prop.apiVersion) << std::endl
				  << "  Driver: " << HK_VERSION_MAJOR(prop.driverVersion) << "." << HK_VERSION_MINOR(prop.driverVersion) << std::endl;
	}

	std::cout << "Picking device 0" << std::endl;
	return devices[0];
}

uint32_t pick_queue_family(HkInstance instance, HkPhysicalDevice phys_dev)
{
	INSTANCE_PROC(instance, hkGetPhysicalDeviceQueueFamilyProperties);

	uint32_t count = 0;
	hkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &count, HK_NULL_HANDLE);

	std::vector<HkQueueFamilyProperties> queue_prop;
	queue_prop.resize(count);
	hkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &count, queue_prop.data());

	std::cout << count << " queue families found." << std::endl;

	for (size_t i = 0; i < count; ++i) {
		HkQueueFamilyProperties prop = queue_prop[i];
		std::cout << "QF[" << i << "]" << std::endl
				  << "  Count:   " << prop.queueCount << std::endl;
	}

	std::cout << "Picking queue familiy 0" << std::endl;
	return 0;
}

HkDevice create_device(HkInstance instance, HkPhysicalDevice pd, uint32_t queue_family)
{
	INSTANCE_PROC(instance, hkCreateDevice);

	float prio = 1;

	HkDeviceQueueCreateInfo queue_info;
	queue_info.sType			= HK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info.pNext			= NULL;
	queue_info.flags			= 0;
	queue_info.queueFamilyIndex = queue_family;
	queue_info.queueCount		= 1;
	queue_info.queuePriority	= &prio;

	HkDeviceCreateInfo info;
	info.sType					 = HK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	info.pNext					 = NULL;
	info.flags					 = 0;
	info.queueCreateInfoCount	= 1;
	info.pQueueCreateInfos		 = &queue_info;
	info.enabledExtensionCount   = 0;
	info.ppEnabledExtensionNames = NULL;
	info.pEnabledFeatures		 = NULL;

	HkDevice device = HK_NULL_HANDLE;
	HkResult result = hkCreateDevice(pd, &info, NULL, &device);
	if (result != HK_SUCCESS)
		return HK_NULL_HANDLE;
	return device;
}

void destroy_device(HkInstance instance, HkDevice device)
{
	INSTANCE_PROC(instance, hkGetDeviceProcAddr);
	DEVICE_PROC(device, hkDestroyDevice);

	if (hkDestroyDevice)
		hkDestroyDevice(device);
}

int main(int argc, char** argv)
{
	HkInstance instance = setup_instance();
	if (instance == HK_NULL_HANDLE)
		return -1;

	HkPhysicalDevice pd = pick_physical_device(instance);
	if (pd == HK_NULL_HANDLE)
		return -2;

	uint32_t queue_family = pick_queue_family(instance, pd);

	HkDevice device = create_device(instance, pd, queue_family);
	if(pd == HK_NULL_HANDLE) {
		destroy_instance(instance);
		return -3;
	}

	destroy_device(instance, device);
	destroy_instance(instance);
	std::cout << "GOOD" << std::endl;
	return 0;
}