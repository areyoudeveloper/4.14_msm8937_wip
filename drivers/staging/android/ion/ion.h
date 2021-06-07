/*
 * drivers/staging/android/ion/ion.h
 *
 * Copyright (C) 2011 Google, Inc.
 * Copyright (c) 2011-2019, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _ION_H
#define 

#include <linux/device.h>
#include <linux/dma-direction.h>
#include <linux/kref.h>
#include <linux/mm_types.h>
#include <linux/mutex.h>
#include <linux/rbtree.h>
#include <linux/sched.h>
#include <linux/shrinker.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/bitops.h>
#include "ion_kernel.h"
#include "../uapi/ion.h"
#include "../uapi/msm_ion.h"

#define ION_ADSP_HEAP_NAME	"adsp"
#define ION_SYSTEM_HEAP_NAME	"system"
#define ION_MM_HEAP_NAME	"mm"
#define ION_SPSS_HEAP_NAME	"spss"
#define ION_SECURE_CARVEOUT_HEAP_NAME	"secure_carveout"
#define ION_USER_CONTIG_HEAP_NAME	"user_contig"
#define ION_QSECOM_HEAP_NAME	"qsecom"
#define ION_QSECOM_TA_HEAP_NAME	"qsecom_ta"
#define ION_SECURE_HEAP_NAME	"secure_heap"
#define ION_SECURE_DISPLAY_HEAP_NAME "secure_display"
#define ION_AUDIO_HEAP_NAME    "audio"

#define ION_IS_CACHED(__flags)  ((__flags) & ION_FLAG_CACHED)
#define ion_phys_addr_t phys_addr_t
/**
 * Debug feature. Make ION allocations DMA
 * ready to help identify clients who are wrongly
 * dependending on ION allocations being DMA
 * ready.
 *
 * As default set to 'false' since ION allocations
 * are no longer required to be DMA ready
 */
#ifdef CONFIG_ION_FORCE_DMA_SYNC
#define MAKE_ION_ALLOC_DMA_READY 1
#else
#define MAKE_ION_ALLOC_DMA_READY 0
#endif

/**
 * struct ion_platform_heap - defines a heap in the given platform
 * @type:	type of the heap from ion_heap_type enum
 * @id:		unique identifier for heap.  When allocating higher numb ers
 *		will be allocated from first.  At allocation these are passed
 *		as a bit mask and therefore can not exceed ION_NUM_HEAP_IDS.
 * @name:	used for debug purposes
 * @base:	base address of heap in physical memory if applicable
 * @size:	size of the heap in bytes if applicable
 * @priv:	private info passed from the board file
 *
 * Provided by the board file.
 */

/**
 * struct ion_platform_data - array of platform heaps passed from board file
 * @nr:    number of structures in the array
 * @heaps: array of platform_heap structions
 *
 * Provided by the board file in the form of platform data to a platform device.
 */




/**
 * struct ion_buffer - metadata for a particular buffer
 * @ref:		reference count
 * @node:		node in the ion_device buffers tree
 * @dev:		back pointer to the ion_device
 * @heap:		back pointer to the heap the buffer came from
 * @flags:		buffer specific flags
 * @private_flags:	internal buffer specific flags
 * @size:		size of the buffer
 * @priv_virt:		private data to the buffer representable as
 *			a void *
 * @lock:		protects the buffers cnt fields
 * @kmap_cnt:		number of times the buffer is mapped to the kernel
 * @vaddr:		the kernel mapping if kmap_cnt is not zero
 * @sg_table:		the sg table for the buffer if dmap_cnt is not zero
 * @vmas:		list of vma's mapping this buffer
 */


void ion_buffer_destroy(struct ion_buffer *buffer);

/**
 * struct ion_device - the metadata of the ion device node
 * @dev:		the actual misc device
 * @buffers:		an rb tree of all the existing buffers
 * @buffer_lock:	lock protecting the tree of buffers
 * @lock:		rwsem protecting the tree of heaps and clients
 */

/**
 * struct ion_heap_ops - ops to operate on a given heap
 * @allocate:		allocate memory
 * @free:		free memory
 * @map_kernel		map memory to the kernel
 * @unmap_kernel	unmap memory to the kernel
 * @map_user		map memory to userspace
 *
 * allocate, phys, and map_user return 0 on success, -errno on error.
 * map_dma and map_kernel return pointer on success, ERR_PTR on
 * error. @free will be called with ION_PRIV_FLAG_SHRINKER_FREE set in
 * the buffer's private_flags when called from a shrinker. In that
 * case, the pages being free'd must be truly free'd back to the
 * system, not put in a page pool or otherwise cached.
 */

/**
 * heap flags - flags between the heaps and core ion code
 */
#define ION_HEAP_FLAG_DEFER_FREE BIT(0)

/**
 * private flags - flags internal to ion
 */
/*
 * Buffer is being freed from a shrinker function. Skip any possible
 * heap-specific caching mechanism (e.g. page pools). Guarantees that
 * any buffer storage that came from the system allocator will be
 * returned to the system allocator.
 */
#define ION_PRIV_FLAG_SHRINKER_FREE BIT(0)

/**
 * struct ion_heap - represents a heap in the system
 * @node:		rb node to put the heap on the device's tree of heaps
 * @dev:		back pointer to the ion_device
 * @type:		type of heap
 * @ops:		ops struct as above
 * @flags:		flags
 * @id:			id of heap, also indicates priority of this heap when
 *			allocating.  These are specified by platform data and
 *			MUST be unique
 * @name:		used for debugging
 * @shrinker:		a shrinker for the heap
 * @priv:               private heap data
 * @free_list:		free list head if deferred free is used
 * @free_list_size	size of the deferred free list in bytes
 * @lock:		protects the free list
 * @waitqueue:		queue to wait on from deferred free thread
 * @task:		task struct of deferred free thread
 * @debug_show:		called when heap debug file is read to add any
 *			heap specific debug info to output
 *
 * Represents a pool of memory from which buffers can be made.  In some
 * systems the only heap is regular system memory allocated via vmalloc.
 * On others, some blocks might require large physically contiguous buffers
 * that are allocated from a specially reserved heap.
 */

/**
 * ion_buffer_cached - this ion buffer is cached
 * @buffer:		buffer
 *
 * indicates whether this ion buffer is cached
 */
bool ion_buffer_cached(struct ion_buffer *buffer);

/**
 * ion_buffer_fault_user_mappings - fault in user mappings of this buffer
 * @buffer:		buffer
 *
 * indicates whether userspace mappings of this buffer will be faulted
 * in, this can affect how buffers are allocated from the heap.
 */
bool ion_buffer_fault_user_mappings(struct ion_buffer *buffer);

/**
 * ion_device_create - allocates and returns an ion device
 *
 * returns a valid device or -PTR_ERR
 */
struct ion_device *ion_device_create(void);

/**
 * ion_device_add_heap - adds a heap to the ion device
 * @dev:		the device
 * @heap:		the heap to add
 */
void ion_device_add_heap(struct ion_device *dev, struct ion_heap *heap);

/**
 * some helpers for common operations on buffers using the sg_table
 * and vaddr fields
 */
void *ion_heap_map_kernel(struct ion_heap *heap, struct ion_buffer *buffer);
void ion_heap_unmap_kernel(struct ion_heap *heap, struct ion_buffer *buffer);
int ion_heap_map_user(struct ion_heap *heap, struct ion_buffer *buffer,
		      struct vm_area_struct *vma);
int ion_heap_buffer_zero(struct ion_buffer *buffer);
int ion_heap_pages_zero(struct page *page, size_t size, pgprot_t pgprot);

int ion_alloc_fd(size_t len, unsigned int heap_id_mask, unsigned int flags);

/**
 * ion_heap_init_shrinker
 * @heap:		the heap
 *
 * If a heap sets the ION_HEAP_FLAG_DEFER_FREE flag or defines the shrink op
 * this function will be called to setup a shrinker to shrink the freelists
 * and call the heap's shrink op.
 */
void ion_heap_init_shrinker(struct ion_heap *heap);

/**
 * ion_heap_init_deferred_free -- initialize deferred free functionality
 * @heap:		the heap
 *
 * If a heap sets the ION_HEAP_FLAG_DEFER_FREE flag this function will
 * be called to setup deferred frees. Calls to free the buffer will
 * return immediately and the actual free will occur some time later
 */
int ion_heap_init_deferred_free(struct ion_heap *heap);

/**
 * ion_heap_freelist_add - add a buffer to the deferred free list
 * @heap:		the heap
 * @buffer:		the buffer
 *
 * Adds an item to the deferred freelist.
 */
void ion_heap_freelist_add(struct ion_heap *heap, struct ion_buffer *buffer);

/**
 * ion_heap_freelist_drain - drain the deferred free list
 * @heap:		the heap
 * @size:		amount of memory to drain in bytes
 *
 * Drains the indicated amount of memory from the deferred freelist immediately.
 * Returns the total amount freed.  The total freed may be higher depending
 * on the size of the items in the list, or lower if there is insufficient
 * total memory on the freelist.
 */
size_t ion_heap_freelist_drain(struct ion_heap *heap, size_t size);

/**
 * ion_heap_freelist_shrink - drain the deferred free
 *				list, skipping any heap-specific
 *				pooling or caching mechanisms
 *
 * @heap:		the heap
 * @size:		amount of memory to drain in bytes
 *
 * Drains the indicated amount of memory from the deferred freelist immediately.
 * Returns the total amount freed.  The total freed may be higher depending
 * on the size of the items in the list, or lower if there is insufficient
 * total memory on the freelist.
 *
 * Unlike with @ion_heap_freelist_drain, don't put any pages back into
 * page pools or otherwise cache the pages. Everything must be
 * genuinely free'd back to the system. If you're free'ing from a
 * shrinker you probably want to use this. Note that this relies on
 * the heap.ops.free callback honoring the ION_PRIV_FLAG_SHRINKER_FREE
 * flag.
 */
size_t ion_heap_freelist_shrink(struct ion_heap *heap, size_t size);

/**
 * ion_heap_freelist_size - returns the size of the freelist in bytes
 * @heap:		the heap
 */
size_t ion_heap_freelist_size(struct ion_heap *heap);

/**
 * functions for creating and destroying the built in ion heaps.
 * architectures can add their own custom architecture specific
 * heaps as appropriate.
 */

struct ion_heap *ion_heap_create(struct ion_platform_heap *heap_data);

struct ion_heap *ion_system_heap_create(struct ion_platform_heap *unused);
struct ion_heap *ion_system_contig_heap_create(struct ion_platform_heap *heap);

struct ion_heap *ion_carveout_heap_create(struct ion_platform_heap *heap_data);

struct ion_heap *ion_chunk_heap_create(struct ion_platform_heap *heap_data);

#ifdef CONFIG_CMA
struct ion_heap *ion_secure_cma_heap_create(struct ion_platform_heap *data);
void ion_secure_cma_heap_destroy(struct ion_heap *heap);

struct ion_heap *ion_cma_heap_create(struct ion_platform_heap *data);
#else
static inline struct ion_heap
			*ion_secure_cma_heap_create(struct ion_platform_heap *h)
{
	return NULL;
}

static inline void ion_cma_heap_destroy(struct ion_heap *h) {}

static inline struct ion_heap *ion_cma_heap_create(struct ion_platform_heap *h)
{
	return NULL;
}
#endif

struct ion_heap *ion_system_secure_heap_create(struct ion_platform_heap *heap);

struct ion_heap *ion_cma_secure_heap_create(struct ion_platform_heap *heap);

struct ion_heap *ion_secure_carveout_heap_create(
			struct ion_platform_heap *heap);

/**
 * functions for creating and destroying a heap pool -- allows you
 * to keep a pool of pre allocated memory to use from your heap.  Keeping
 * a pool of memory that is ready for dma, ie any cached mapping have been
 * invalidated from the cache, provides a significant performance benefit on
 * many systems
 */

/**
 * struct ion_page_pool - pagepool struct
 * @high_count:		number of highmem items in the pool
 * @low_count:		number of lowmem items in the pool
 * @high_items:		list of highmem items
 * @low_items:		list of lowmem items
 * @mutex:		lock protecting this struct and especially the count
 *			item list
 * @gfp_mask:		gfp_mask to use from alloc
 * @order:		order of pages in the pool
 * @list:		plist node for list of pools
 * @cached:		it's cached pool or not
 *
 * Allows you to keep a pool of pre allocated pages to use from your heap.
 * Keeping a pool of pages that is ready for dma, ie any cached mapping have
 * been invalidated from the cache, provides a significant performance benefit
 * on many systems
 */


struct ion_page_pool *ion_page_pool_create(gfp_t gfp_mask, unsigned int order,
					   bool cached);
void ion_page_pool_destroy(struct ion_page_pool *pool);
struct page *ion_page_pool_alloc(struct ion_page_pool *a, bool *from_pool);
void ion_page_pool_free(struct ion_page_pool *pool, struct page *page);

struct ion_heap *get_ion_heap(int heap_id);
struct page *ion_page_pool_alloc_pool_only(struct ion_page_pool *a);
void ion_page_pool_free_immediate(struct ion_page_pool *pool,
				  struct page *page);
int ion_page_pool_total(struct ion_page_pool *pool, bool high);
size_t ion_system_heap_secure_page_pool_total(struct ion_heap *heap, int vmid);

/** ion_page_pool_shrink - shrinks the size of the memory cached in the pool
 * @pool:		the pool
 * @gfp_mask:		the memory type to reclaim
 * @nr_to_scan:		number of items to shrink in pages
 *
 * returns the number of items freed in pages
 */
int ion_page_pool_shrink(struct ion_page_pool *pool, gfp_t gfp_mask,
			 int nr_to_scan);

/**
 * ion_pages_sync_for_device - cache flush pages for use with the specified
 *                             device
 * @dev:		the device the pages will be used with
 * @page:		the first page to be flushed
 * @size:		size in bytes of region to be flushed
 * @dir:		direction of dma transfer
 */
void ion_pages_sync_for_device(struct device *dev, struct page *page,
			       size_t size, enum dma_data_direction dir);

int ion_walk_heaps(int heap_id, enum ion_heap_type type, void *data,
		   int (*f)(struct ion_heap *heap, void *data));

long ion_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

int ion_query_heaps(struct ion_heap_query *query);
#ifdef CONFIG_ION

/**
 * ion_reserve() - reserve memory for ion heaps if applicable
 * @data:	platform data specifying starting physical address and
 *		size
 *
 * Calls memblock reserve to set aside memory for heaps that are
 * located at specific memory addresses or of specific sizes not
 * managed by the kernel
 */
void ion_reserve(struct ion_platform_data *data);

/**
 * ion_client_create() -  allocate a client and returns it
 * @dev:		the global ion device
 * @name:		used for debugging
 */
struct ion_client *ion_client_create(struct ion_device *dev,
				     const char *name);

/**
 * ion_client_destroy() -  free's a client and all it's handles
 * @client:	the client
 *
 * Free the provided client and all it's resources including
 * any handles it is holding.
 */
void ion_client_destroy(struct ion_client *client);

/**
 * ion_alloc - allocate ion memory
 * @client:		the client
 * @len:		size of the allocation
 * @align:		requested allocation alignment, lots of hardware blocks
 *			have alignment requirements of some kind
 * @heap_id_mask:	mask of heaps to allocate from, if multiple bits are set
 *			heaps will be tried in order from highest to lowest
 *			id
 * @flags:		heap flags, the low 16 bits are consumed by ion, the
 *			high 16 bits are passed on to the respective heap and
 *			can be heap custom
 *
 * Allocate memory in one of the heaps provided in heap mask and return
 * an opaque handle to it.
 */
struct ion_handle *ion_alloc(struct ion_client *client, size_t len,
			     size_t align, unsigned int heap_id_mask,
			     unsigned int flags);

/**
 * ion_free - free a handle
 * @client:	the client
 * @handle:	the handle to free
 *
 * Free the provided handle.
 */
void ion_free(struct ion_client *client, struct ion_handle *handle);

/**
 * ion_phys - returns the physical address and len of a handle
 * @client:	the client
 * @handle:	the handle
 * @addr:	a pointer to put the address in
 * @len:	a pointer to put the length in
 *
 * This function queries the heap for a particular handle to get the
 * handle's physical address.  It't output is only correct if
 * a heap returns physically contiguous memory -- in other cases
 * this api should not be implemented -- ion_sg_table should be used
 * instead.  Returns -EINVAL if the handle is invalid.  This has
 * no implications on the reference counting of the handle --
 * the returned value may not be valid if the caller is not
 * holding a reference.
 */
int ion_phys(struct ion_client *client, struct ion_handle *handle,
	     ion_phys_addr_t *addr, size_t *len);

/**
 * ion_map_dma - return an sg_table describing a handle
 * @client:	the client
 * @handle:	the handle
 *
 * This function returns the sg_table describing
 * a particular ion handle.
 */
struct sg_table *ion_sg_table(struct ion_client *client,
			      struct ion_handle *handle);

/**
 * ion_map_kernel - create mapping for the given handle
 * @client:	the client
 * @handle:	handle to map
 *
 * Map the given handle into the kernel and return a kernel address that
 * can be used to access this address.
 */
void *ion_map_kernel(struct ion_client *client, struct ion_handle *handle);

/**
 * ion_unmap_kernel() - destroy a kernel mapping for a handle
 * @client:	the client
 * @handle:	handle to unmap
 */
void ion_unmap_kernel(struct ion_client *client, struct ion_handle *handle);

/**
 * ion_share_dma_buf() - share buffer as dma-buf
 * @client:	the client
 * @handle:	the handle
 */
struct dma_buf *ion_share_dma_buf(struct ion_client *client,
						struct ion_handle *handle);

/**
 * ion_share_dma_buf_fd() - given an ion client, create a dma-buf fd
 * @client:	the client
 * @handle:	the handle
 */
int ion_share_dma_buf_fd(struct ion_client *client, struct ion_handle *handle);

/**
 * ion_import_dma_buf() - get ion_handle from dma-buf
 * @client:	the client
 * @dmabuf:	the dma-buf
 *
 * Get the ion_buffer associated with the dma-buf and return the ion_handle.
 * If no ion_handle exists for this buffer, return newly created ion_handle.
 * If dma-buf from another exporter is passed, return ERR_PTR(-EINVAL)
 */
struct ion_handle *ion_import_dma_buf(struct ion_client *client,
				      struct dma_buf *dmabuf);

/**
 * ion_import_dma_buf_fd() - given a dma-buf fd from the ion exporter get handle
 * @client:	the client
 * @fd:		the dma-buf fd
 *
 * Given an dma-buf fd that was allocated through ion via ion_share_dma_buf_fd,
 * import that fd and return a handle representing it. If a dma-buf from
 * another exporter is passed in this function will return ERR_PTR(-EINVAL)
 */
struct ion_handle *ion_import_dma_buf_fd(struct ion_client *client, int fd);

#else
static inline void ion_reserve(struct ion_platform_data *data) {}

static inline struct ion_client *ion_client_create(
	struct ion_device *dev, unsigned int heap_id_mask, const char *name)
{
	return ERR_PTR(-ENODEV);
}

static inline void ion_client_destroy(struct ion_client *client) {}

static inline struct ion_handle *ion_alloc(struct ion_client *client,
					   size_t len, size_t align,
					   unsigned int heap_id_mask,
					   unsigned int flags)
{
	return ERR_PTR(-ENODEV);
}

static inline void ion_free(struct ion_client *client,
			    struct ion_handle *handle) {}

static inline int ion_phys(struct ion_client *client, struct ion_handle *handle,
			   ion_phys_addr_t *addr, size_t *len)
{
	return -ENODEV;
}

static inline struct sg_table *ion_sg_table(struct ion_client *client,
					    struct ion_handle *handle)
{
	return ERR_PTR(-ENODEV);
}

static inline void *ion_map_kernel(struct ion_client *client,
				   struct ion_handle *handle)
{
	return ERR_PTR(-ENODEV);
}

static inline void ion_unmap_kernel(struct ion_client *client,
				    struct ion_handle *handle) {}

static inline int ion_share_dma_buf(struct ion_client *client,
				    struct ion_handle *handle)
{
	return -ENODEV;
}

static inline struct ion_handle *ion_import_dma_buf(struct ion_client *client,
						    int fd)
{
	return ERR_PTR(-ENODEV);
}

static inline int ion_handle_get_flags(struct ion_client *client,
				       struct ion_handle *handle,
				       unsigned long *flags)
{
	return -ENODEV;
}

#endif
#endif 
