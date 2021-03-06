/*
 * Copyright 2006-2009, Antares, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Axel Dörfler, axeld@pinc-software.de
 */


#include "domains.h"
#include "interfaces.h"
#include "stack_private.h"
#include "utility.h"

#include <net_device.h>

#include <lock.h>
#include <util/AutoLock.h>

#include <KernelExport.h>

#include <net/if_dl.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TRACE_INTERFACES
#ifdef TRACE_INTERFACES
#	define TRACE(x) dprintf x
#else
#	define TRACE(x) ;
#endif

#define ENABLE_DEBUGGER_COMMANDS	1


static mutex sInterfaceLock;
static DeviceInterfaceList sInterfaces;
static uint32 sInterfaceIndex;
static uint32 sDeviceIndex;


static status_t
device_consumer_thread(void* _interface)
{
	net_device_interface* interface = (net_device_interface*)_interface;
	net_device* device = interface->device;
	net_buffer* buffer;

	while (true) {
		ssize_t status = fifo_dequeue_buffer(&interface->receive_queue, 0,
			B_INFINITE_TIMEOUT, &buffer);
		if (status == B_INTERRUPTED)
			continue;
		else if (status < B_OK)
			break;

		if (buffer->interface != NULL) {
			// if the interface is already specified, this buffer was
			// delivered locally.
			if (buffer->interface->domain->module->receive_data(buffer) == B_OK)
				buffer = NULL;
		} else {
			// find handler for this packet
			DeviceHandlerList::Iterator iterator =
				interface->receive_funcs.GetIterator();
			while (buffer && iterator.HasNext()) {
				net_device_handler* handler = iterator.Next();

				// if the handler returns B_OK, it consumed the buffer
				if (handler->type == buffer->type
					&& handler->func(handler->cookie, device, buffer) == B_OK)
					buffer = NULL;
			}
		}

		if (buffer != NULL)
			gNetBufferModule.free(buffer);
	}

	return B_OK;
}


static net_device_interface*
find_device_interface(const char* name)
{
	DeviceInterfaceList::Iterator iterator = sInterfaces.GetIterator();

	while (net_device_interface* interface = iterator.Next()) {
		if (!strcmp(interface->device->name, name))
			return interface;
	}

	return NULL;
}


/*!	The domain's device receive handler - this will inject the net_buffers into
	the protocol layer (the domain's registered receive handler).
*/
static status_t
domain_receive_adapter(void* cookie, net_device* device, net_buffer* buffer)
{
	net_domain_private* domain = (net_domain_private*)cookie;

	buffer->interface = find_interface(domain, device->index);
	return domain->module->receive_data(buffer);
}


static net_device_interface*
allocate_device_interface(net_device* device, net_device_module_info* module)
{
	net_device_interface* interface = new(std::nothrow) net_device_interface;
	if (interface == NULL)
		return NULL;

	recursive_lock_init(&interface->receive_lock, "interface receive lock");

	char name[128];
	snprintf(name, sizeof(name), "%s receive queue", device->name);

	if (init_fifo(&interface->receive_queue, name, 16 * 1024 * 1024) < B_OK)
		goto error1;

	interface->device = device;
	interface->up_count = 0;
	interface->ref_count = 1;
	interface->deframe_func = NULL;
	interface->deframe_ref_count = 0;

	snprintf(name, sizeof(name), "%s consumer", device->name);

	interface->reader_thread   = -1;
	interface->consumer_thread = spawn_kernel_thread(device_consumer_thread,
		name, B_DISPLAY_PRIORITY, interface);
	if (interface->consumer_thread < B_OK)
		goto error2;
	resume_thread(interface->consumer_thread);

	// TODO: proper interface index allocation
	device->index = ++sDeviceIndex;
	device->module = module;

	sInterfaces.Add(interface);
	return interface;

error2:
	uninit_fifo(&interface->receive_queue);
error1:
	recursive_lock_destroy(&interface->receive_lock);
	delete interface;

	return NULL;
}


static net_device_interface*
acquire_device_interface(net_device_interface* interface)
{
	if (interface == NULL || atomic_add(&interface->ref_count, 1) == 0)
		return NULL;

	return interface;
}


static void
notify_device_monitors(net_device_interface* interface, int32 event)
{
	RecursiveLocker _(interface->receive_lock);

	DeviceMonitorList::Iterator iterator
		= interface->monitor_funcs.GetIterator();
	while (net_device_monitor* monitor = iterator.Next()) {
		// it's safe for the "current" item to remove itself.
		monitor->event(monitor, event);
	}
}


#if ENABLE_DEBUGGER_COMMANDS


static int
dump_interface(int argc, char** argv)
{
	if (argc != 2) {
		kprintf("usage: %s [address]\n", argv[0]);
		return 0;
	}

	net_interface_private* interface
		= (net_interface_private*)parse_expression(argv[1]);

	kprintf("name:             %s\n", interface->name);
	kprintf("base_name:        %s\n", interface->name);
	kprintf("domain:           %p\n", interface->domain);
	kprintf("device:           %p\n", interface->device);
	kprintf("device_interface: %p\n", interface->device_interface);
	kprintf("direct_route:     %p\n", &interface->direct_route);
	kprintf("first_protocol:   %p\n", interface->first_protocol);
	kprintf("first_info:       %p\n", interface->first_info);
	kprintf("address:          %p\n", interface->address);
	kprintf("destination:      %p\n", interface->destination);
	kprintf("mask:             %p\n", interface->mask);
	kprintf("index:            %" B_PRIu32 "\n", interface->index);
	kprintf("flags:            %#" B_PRIx32 "\n", interface->flags);
	kprintf("type:             %u\n", interface->type);
	kprintf("mtu:              %" B_PRIu32 "\n", interface->mtu);
	kprintf("metric:           %" B_PRIu32 "\n", interface->metric);

	return 0;
}


static int
dump_device_interface(int argc, char** argv)
{
	if (argc != 2) {
		kprintf("usage: %s [address]\n", argv[0]);
		return 0;
	}

	net_device_interface* interface
		= (net_device_interface*)parse_expression(argv[1]);

	kprintf("device:            %p\n", interface->device);
	kprintf("reader_thread:     %ld\n", interface->reader_thread);
	kprintf("up_count:          %" B_PRIu32 "\n", interface->up_count);
	kprintf("ref_count:         %" B_PRId32 "\n", interface->ref_count);
	kprintf("deframe_func:      %p\n", interface->deframe_func);
	kprintf("deframe_ref_count: %" B_PRId32 "\n", interface->ref_count);
	kprintf("monitor_funcs:\n");
	kprintf("receive_funcs:\n");
	kprintf("consumer_thread:   %ld\n", interface->consumer_thread);
	kprintf("receive_lock:      %p\n", &interface->receive_lock);
	kprintf("receive_queue:     %p\n", &interface->receive_queue);

	DeviceMonitorList::Iterator monitorIterator
		= interface->monitor_funcs.GetIterator();
	while (monitorIterator.HasNext())
		kprintf("  %p\n", monitorIterator.Next());

	DeviceHandlerList::Iterator handlerIterator
		= interface->receive_funcs.GetIterator();
	while (handlerIterator.HasNext())
		kprintf("  %p\n", handlerIterator.Next());

	return 0;
}


static int
dump_device_interfaces(int argc, char** argv)
{
	DeviceInterfaceList::Iterator iterator = sInterfaces.GetIterator();
	while (net_device_interface* interface = iterator.Next()) {
		kprintf("  %p\n", interface);
	}

	return 0;
}


#endif	// ENABLE_DEBUGGER_COMMANDS


//	#pragma mark - interfaces


/*!	Searches for a specific interface in a domain by name.
	You need to have the domain's lock hold when calling this function.
*/
struct net_interface_private*
find_interface(struct net_domain* domain, const char* name)
{
	net_interface_private* interface = NULL;

	while (true) {
		interface = (net_interface_private*)list_get_next_item(
			&domain->interfaces, interface);
		if (interface == NULL)
			break;

		if (!strcmp(interface->name, name))
			return interface;
	}

	return NULL;
}


/*!	Searches for a specific interface in a domain by index.
	You need to have the domain's lock hold when calling this function.
*/
struct net_interface_private*
find_interface(struct net_domain* domain, uint32 index)
{
	net_interface_private* interface = NULL;

	while (true) {
		interface = (net_interface_private*)list_get_next_item(
			&domain->interfaces, interface);
		if (interface == NULL)
			break;

		if (interface->index == index)
			return interface;
	}

	return NULL;
}


status_t
create_interface(net_domain* domain, const char* name, const char* baseName,
	net_device_interface* deviceInterface, net_interface_private** _interface)
{
	net_interface_private* interface = new(std::nothrow) net_interface_private;
	if (interface == NULL)
		return B_NO_MEMORY;

	strlcpy(interface->name, name, IF_NAMESIZE);
	strlcpy(interface->base_name, baseName, IF_NAMESIZE);
	interface->domain = domain;
	interface->device = deviceInterface->device;

	interface->address = NULL;
	interface->destination = NULL;
	interface->mask = NULL;

	interface->index = ++sInterfaceIndex;
	interface->flags = 0;
	interface->type = 0;
	interface->mtu = deviceInterface->device->mtu;
	interface->metric = 0;
	interface->device_interface = acquire_device_interface(deviceInterface);

	// setup direct route for bound devices
	interface->direct_route.destination = NULL;
	interface->direct_route.mask = NULL;
	interface->direct_route.gateway = NULL;
	interface->direct_route.flags = 0;
	interface->direct_route.mtu = 0;
	interface->direct_route.interface = interface;
	interface->direct_route.ref_count = 1;
		// make sure this doesn't get deleted accidently

	status_t status = get_domain_datalink_protocols(interface);
	if (status < B_OK) {
		delete interface;
		return status;
	}

	// Grab a reference to the networking stack, to make sure it won't be
	// unloaded as long as an interface exists
	module_info* module;
	get_module(gNetStackInterfaceModule.info.name, &module);

	*_interface = interface;
	return B_OK;
}


void
interface_set_down(net_interface* interface)
{
	if ((interface->flags & IFF_UP) == 0)
		return;

	interface->flags &= ~IFF_UP;
	interface->first_info->interface_down(interface->first_protocol);
}


void
delete_interface(net_interface_private* interface)
{
	// deleting an interface is fairly complex as we need
	// to clear all references to it throughout the stack

	// this will possibly call (if IFF_UP):
	//  interface_protocol_down()
	//   domain_interface_went_down()
	//    invalidate_routes()
	//     remove_route()
	//      update_route_infos()
	//       get_route_internal()
	//   down_device_interface() -- if upcount reaches 0
	interface_set_down(interface);

	// This call requires the RX Lock to be a recursive
	// lock since each uninit_protocol() call may call
	// again into the stack to unregister a reader for
	// instance, which tries to obtain the RX lock again.
	put_domain_datalink_protocols(interface);

	put_device_interface(interface->device_interface);

	free(interface->address);
	free(interface->destination);
	free(interface->mask);

	delete interface;

	// Release reference of the stack - at this point, our stack may be unloaded
	// if no other interfaces or sockets are left
	put_module(gNetStackInterfaceModule.info.name);
}


void
put_interface(struct net_interface_private* interface)
{
	// TODO: reference counting
	// TODO: better locking scheme
	recursive_lock_unlock(&((net_domain_private*)interface->domain)->lock);
}


struct net_interface_private*
get_interface(net_domain* _domain, const char* name)
{
	net_domain_private* domain = (net_domain_private*)_domain;
	recursive_lock_lock(&domain->lock);

	net_interface_private* interface = NULL;
	while (true) {
		interface = (net_interface_private*)list_get_next_item(
			&domain->interfaces, interface);
		if (interface == NULL)
			break;

		// TODO: We keep the domain locked for now
		if (!strcmp(interface->name, name))
			return interface;
	}

	recursive_lock_unlock(&domain->lock);
	return NULL;
}


//	#pragma mark - device interfaces


void
get_device_interface_address(net_device_interface* interface, sockaddr* _address)
{
	sockaddr_dl &address = *(sockaddr_dl*)_address;

	address.sdl_family = AF_LINK;
	address.sdl_index = interface->device->index;
	address.sdl_type = interface->device->type;
	address.sdl_nlen = strlen(interface->device->name);
	address.sdl_slen = 0;
	memcpy(address.sdl_data, interface->device->name, address.sdl_nlen);

	address.sdl_alen = interface->device->address.length;
	memcpy(LLADDR(&address), interface->device->address.data, address.sdl_alen);

	address.sdl_len = sizeof(sockaddr_dl) - sizeof(address.sdl_data)
		+ address.sdl_nlen + address.sdl_alen;
}


uint32
count_device_interfaces()
{
	MutexLocker locker(sInterfaceLock);

	DeviceInterfaceList::Iterator iterator = sInterfaces.GetIterator();
	uint32 count = 0;

	while (iterator.HasNext()) {
		iterator.Next();
		count++;
	}

	return count;
}


/*!	Dumps a list of all interfaces into the supplied userland buffer.
	If the interfaces don't fit into the buffer, an error (\c ENOBUFS) is
	returned.
*/
status_t
list_device_interfaces(void* _buffer, size_t* bufferSize)
{
	MutexLocker locker(sInterfaceLock);

	DeviceInterfaceList::Iterator iterator = sInterfaces.GetIterator();
	UserBuffer buffer(_buffer, *bufferSize);

	while (net_device_interface* interface = iterator.Next()) {
		ifreq request;
		strlcpy(request.ifr_name, interface->device->name, IF_NAMESIZE);
		get_device_interface_address(interface, &request.ifr_addr);

		if (buffer.Copy(&request, IF_NAMESIZE + request.ifr_addr.sa_len) == NULL)
			return buffer.Status();
	}

	*bufferSize = buffer.ConsumedAmount();
	return B_OK;
}


/*!	Releases the reference for the interface. When all references are
	released, the interface is removed.
*/
void
put_device_interface(struct net_device_interface* interface)
{
	if (atomic_add(&interface->ref_count, -1) != 1)
		return;

	{
		MutexLocker locker(sInterfaceLock);
		sInterfaces.Remove(interface);
	}

	uninit_fifo(&interface->receive_queue);
	status_t status;
	wait_for_thread(interface->consumer_thread, &status);

	net_device* device = interface->device;
	const char* moduleName = device->module->info.name;

	device->module->uninit_device(device);
	put_module(moduleName);

	recursive_lock_destroy(&interface->receive_lock);
	delete interface;
}


/*!	Finds an interface by the specified index and acquires a reference to it.
*/
struct net_device_interface*
get_device_interface(uint32 index)
{
	MutexLocker locker(sInterfaceLock);

	// TODO: maintain an array of all device interfaces instead
	DeviceInterfaceList::Iterator iterator = sInterfaces.GetIterator();
	while (net_device_interface* interface = iterator.Next()) {
		if (interface->device->index == index) {
			if (atomic_add(&interface->ref_count, 1) != 0)
				return interface;
		}
	}

	return NULL;
}


/*!	Finds an interface by the specified name and grabs a reference to it.
	If the interface does not yet exist, a new one is created.
*/
struct net_device_interface*
get_device_interface(const char* name, bool create)
{
	MutexLocker locker(sInterfaceLock);

	net_device_interface* interface = find_device_interface(name);
	if (interface != NULL) {
		if (atomic_add(&interface->ref_count, 1) != 0)
			return interface;

		// try to recreate interface - it just got removed
	}

	if (!create)
		return NULL;

	void* cookie = open_module_list("network/devices");
	if (cookie == NULL)
		return NULL;

	while (true) {
		char moduleName[B_FILE_NAME_LENGTH];
		size_t length = sizeof(moduleName);
		if (read_next_module_name(cookie, moduleName, &length) != B_OK)
			break;

		TRACE(("get_device_interface: ask \"%s\" for %s\n", moduleName, name));

		net_device_module_info* module;
		if (get_module(moduleName, (module_info**)&module) == B_OK) {
			net_device* device;
			status_t status = module->init_device(name, &device);
			if (status == B_OK) {
				interface = allocate_device_interface(device, module);
				if (interface != NULL)
					return interface;

				module->uninit_device(device);
			}
			put_module(moduleName);
		}
	}

	return NULL;
}


void
down_device_interface(net_device_interface* interface)
{
	// Receive lock must be held when calling down_device_interface.
	// Known callers are `interface_protocol_down' which gets
	// here via one of the following paths:
	//
	// - domain_interface_control() [rx lock held, domain lock held]
	//    interface_set_down()
	//     interface_protocol_down()
	//
	// - domain_interface_control() [rx lock held, domain lock held]
	//    remove_interface_from_domain()
	//     delete_interface()
	//      interface_set_down()

	net_device* device = interface->device;

	device->flags &= ~IFF_UP;
	device->module->down(device);

	notify_device_monitors(interface, B_DEVICE_GOING_DOWN);

	if (device->module->receive_data != NULL) {
		thread_id readerThread = interface->reader_thread;

		// make sure the reader thread is gone before shutting down the interface
		status_t status;
		wait_for_thread(readerThread, &status);
	}
}


//	#pragma mark - devices stack API


/*!	Unregisters a previously registered deframer function. */
status_t
unregister_device_deframer(net_device* device)
{
	MutexLocker locker(sInterfaceLock);

	// find device interface for this device
	net_device_interface* interface = find_device_interface(device->name);
	if (interface == NULL)
		return ENODEV;

	RecursiveLocker _(interface->receive_lock);

	if (--interface->deframe_ref_count == 0)
		interface->deframe_func = NULL;

	return B_OK;
}


/*!	Registers the deframer function for the specified \a device.
	Note, however, that right now, you can only register one single
	deframer function per device.

	If the need arises, we might want to lift that limitation at a
	later time (which would require a slight API change, though).
*/
status_t
register_device_deframer(net_device* device, net_deframe_func deframeFunc)
{
	MutexLocker locker(sInterfaceLock);

	// find device interface for this device
	net_device_interface* interface = find_device_interface(device->name);
	if (interface == NULL)
		return ENODEV;

	RecursiveLocker _(interface->receive_lock);

	if (interface->deframe_func != NULL
		&& interface->deframe_func != deframeFunc)
		return B_ERROR;

	interface->deframe_func = deframeFunc;
	interface->deframe_ref_count++;
	return B_OK;
}


/*!	Registers a domain to receive net_buffers from the specified \a device. */
status_t
register_domain_device_handler(struct net_device* device, int32 type,
	struct net_domain* _domain)
{
	net_domain_private* domain = (net_domain_private*)_domain;
	if (domain->module == NULL || domain->module->receive_data == NULL)
		return B_BAD_VALUE;

	return register_device_handler(device, type, &domain_receive_adapter,
		domain);
}


/*!	Registers a receiving function callback for the specified \a device. */
status_t
register_device_handler(struct net_device* device, int32 type,
	net_receive_func receiveFunc, void* cookie)
{
	MutexLocker locker(sInterfaceLock);

	// find device interface for this device
	net_device_interface* interface = find_device_interface(device->name);
	if (interface == NULL)
		return ENODEV;

	RecursiveLocker _(interface->receive_lock);

	// see if such a handler already for this device

	DeviceHandlerList::Iterator iterator
		= interface->receive_funcs.GetIterator();
	while (net_device_handler* handler = iterator.Next()) {
		if (handler->type == type)
			return B_ERROR;
	}

	// Add new handler

	net_device_handler* handler = new(std::nothrow) net_device_handler;
	if (handler == NULL)
		return B_NO_MEMORY;

	handler->func = receiveFunc;
	handler->type = type;
	handler->cookie = cookie;
	interface->receive_funcs.Add(handler);
	return B_OK;
}


/*!	Unregisters a previously registered device handler. */
status_t
unregister_device_handler(struct net_device* device, int32 type)
{
	MutexLocker locker(sInterfaceLock);

	// find device interface for this device
	net_device_interface* interface = find_device_interface(device->name);
	if (interface == NULL)
		return ENODEV;

	RecursiveLocker _(interface->receive_lock);

	// search for the handler

	DeviceHandlerList::Iterator iterator
		= interface->receive_funcs.GetIterator();
	while (net_device_handler* handler = iterator.Next()) {
		if (handler->type == type) {
			// found it
			iterator.Remove();
			delete handler;
			return B_OK;
		}
	}

	return B_BAD_VALUE;
}


/*!	Registers a device monitor for the specified device. */
status_t
register_device_monitor(net_device* device, net_device_monitor* monitor)
{
	if (monitor->receive == NULL || monitor->event == NULL)
		return B_BAD_VALUE;

	MutexLocker locker(sInterfaceLock);

	// find device interface for this device
	net_device_interface* interface = find_device_interface(device->name);
	if (interface == NULL)
		return ENODEV;

	RecursiveLocker _(interface->receive_lock);
	interface->monitor_funcs.Add(monitor);
	return B_OK;
}


/*!	Unregisters a previously registered device monitor. */
status_t
unregister_device_monitor(net_device* device, net_device_monitor* monitor)
{
	MutexLocker locker(sInterfaceLock);

	// find device interface for this device
	net_device_interface* interface = find_device_interface(device->name);
	if (interface == NULL)
		return ENODEV;

	RecursiveLocker _(interface->receive_lock);

	// search for the monitor

	DeviceMonitorList::Iterator iterator = interface->monitor_funcs.GetIterator();
	while (iterator.HasNext()) {
		if (iterator.Next() == monitor) {
			iterator.Remove();
			return B_OK;
		}
	}

	return B_BAD_VALUE;
}


/*!	This function is called by device modules in case their link
	state changed, ie. if an ethernet cable was plugged in or
	removed.
*/
status_t
device_link_changed(net_device* device)
{
	notify_link_changed(device);
	return B_OK;
}


/*!	This function is called by device modules once their device got
	physically removed, ie. a USB networking card is unplugged.
*/
status_t
device_removed(net_device* device)
{
	MutexLocker locker(sInterfaceLock);

	// hold a reference to the device interface being removed
	// so our put_() will (eventually) do the final cleanup
	net_device_interface* interface = get_device_interface(device->name, false);
	if (interface == NULL)
		return ENODEV;

	// Propagate the loss of the device throughout the stack.
	// This is very complex, refer to delete_interface() for
	// further details.

	domain_removed_device_interface(interface);

	notify_device_monitors(interface, B_DEVICE_BEING_REMOVED);

	// By now all of the monitors must have removed themselves. If they
	// didn't, they'll probably wait forever to be callback'ed again.
	interface->monitor_funcs.RemoveAll();

	// All of the readers should be gone as well since we are out of
	// interfaces and put_domain_datalink_protocols() is called for
	// each delete_interface().

	put_device_interface(interface);

	return B_OK;
}


status_t
device_enqueue_buffer(net_device* device, net_buffer* buffer)
{
	net_device_interface* interface = get_device_interface(device->index);
	if (interface == NULL)
		return ENODEV;

	status_t status = fifo_enqueue_buffer(&interface->receive_queue, buffer);

	put_device_interface(interface);
	return status;
}


//	#pragma mark -


status_t
init_interfaces()
{
	mutex_init(&sInterfaceLock, "net interfaces");

	new (&sInterfaces) DeviceInterfaceList;
		// static C++ objects are not initialized in the module startup

#if ENABLE_DEBUGGER_COMMANDS
	add_debugger_command("net_interface", &dump_interface,
		"Dump the given network interface");
	add_debugger_command("net_device_interface", &dump_device_interface,
		"Dump the given network device interface");
	add_debugger_command("net_device_interfaces", &dump_device_interfaces,
		"Dump network device interfaces");
#endif
	return B_OK;
}


status_t
uninit_interfaces()
{
#if ENABLE_DEBUGGER_COMMANDS
	remove_debugger_command("net_interface", &dump_interface);
	remove_debugger_command("net_device_interface", &dump_device_interface);
	remove_debugger_command("net_device_interfaces", &dump_device_interfaces);
#endif

	mutex_destroy(&sInterfaceLock);
	return B_OK;
}

