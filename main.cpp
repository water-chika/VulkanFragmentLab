#include <vulkan/vulkan.hpp>
#include "glfwpp.hpp"
#include <set>
#include <vector>

int main() {
	bool init_res = glfwpp::init();
	assert(init_res);

	glfwpp::window_hint(GLFW_CLIENT_API, GLFW_NO_API);
	auto window = glfwpp::create_window(151, 431, "fragment lab", NULL, NULL);
	assert(window);

	uint32_t glfw_required_instance_extension_count{};
	const char* const* glfw_required_instance_extensions = glfwpp::get_required_instance_extensions(&glfw_required_instance_extension_count);

	std::set<std::string> instance_extension_set{};
	for (auto extension : std::span{ glfw_required_instance_extensions, glfw_required_instance_extension_count })
	{
		instance_extension_set.emplace(extension);
	}

	std::vector<std::string> instance_extension_vector(instance_extension_set.size());
	std::ranges::copy(
		instance_extension_set, instance_extension_vector.begin());
	std::vector<const char*> instance_extensions(instance_extension_vector.size());
	std::ranges::transform(
		instance_extension_vector,
		instance_extensions.begin(),
		[](auto& str) {
			return str.c_str();
		}
	);

	auto instance = vk::createInstance(
		vk::InstanceCreateInfo{}
			.setPEnabledExtensionNames(instance_extensions)
	);

	VkSurfaceKHR surface_raw{};

	VkResult res = glfwpp::create_surface(instance, window, nullptr, &surface_raw);
	assert(res == VK_SUCCESS);

	vk::SurfaceKHR surface = surface_raw;

	auto physical_device = instance.enumeratePhysicalDevices()[0];
	float queue_priority = 1.0f;
	auto queue_create_info = 
		vk::DeviceQueueCreateInfo{}
			.setPQueuePriorities(&queue_priority)
			.setQueueCount(1)
			.setQueueFamilyIndex(0);
	std::vector<const char*> device_extensions{
		vk::KHRSwapchainExtensionName
	};
	auto device = physical_device.createDevice(
		vk::DeviceCreateInfo{}
		.setQueueCreateInfos(queue_create_info)
		.setPEnabledExtensionNames(device_extensions)
	);

	auto queue = device.getQueue(0, 0);

	auto command_pool = device.createCommandPool(
		vk::CommandPoolCreateInfo{}.setQueueFamilyIndex(0)
	);

	auto command_buffer = device.allocateCommandBuffers(
		vk::CommandBufferAllocateInfo{}
		.setCommandPool(command_pool)
		.setCommandBufferCount(1)
	)[0];

	auto surface_capability = physical_device.getSurfaceCapabilitiesKHR(surface);
	auto surface_formats = physical_device.getSurfaceFormatsKHR(surface);
	auto swapchain_format = vk::Format::eR8G8B8A8Unorm;
	assert(
		std::ranges::contains(surface_formats, swapchain_format,
			[](auto surface_format) {
				return surface_format.format;
			}));

	auto swapchain = device.createSwapchainKHR(
		vk::SwapchainCreateInfoKHR{}.setImageFormat(swapchain_format)
		.setImageExtent(surface_capability.currentExtent)
		.setMinImageCount(3)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setImageArrayLayers(1)
		.setSurface(surface)
	);

	

	while (!glfwpp::window_should_close(window)) {
		glfwpp::poll_events();
	}


	device.destroySwapchainKHR(swapchain);

	device.destroyCommandPool(command_pool);

	device.destroy();

	instance.destroySurfaceKHR(surface);

	instance.destroy();

	glfwpp::destroy_window(window);

	glfwpp::terminate();
	return 0;
}
