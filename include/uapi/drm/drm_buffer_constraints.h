/*
 * Copyright (c) 2023 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef DRM_BUFFER_CONSTRAINT
#define DRM_BUFFER_CONSTRAINT

#include "drm.h"

/* TODO(Weifeng): Add versioning support? */

/**
 * @brief APIs for reporting buffer constraint of the device to user-space.
 * 
 * Default values of the constraint
 */
enum drm_buffer_constraint_id {
	/* Predefind vendor agnostic type id */

	/**
	 * @DRM_BUFFER_CONSTRAINT_PLACEMENT, where the buffer should reside,
	 * mandetory and only reported placements are acceptable to the device.
	 */
	DRM_BUFFER_CONSTRAINT_PLACEMENT         = 1,
	DRM_BUFFER_CONSTRAINT_ADDRESS_ALIGNMENT = 2,
	DRM_BUFFER_CONSTRAINT_ADDRESS_MASK      = 3,
	DRM_BUFFER_CONSTRAINT_CACHE             = 4,
	DRM_BUFFER_CONSTRAINT_CONTIGUOUS        = 5,
};

/**
 * Placement constraints are hard to express because not all constraints are 
 * caused by hardware limitations; instead, some of them are imposed by
 * software (think about DRM driver not implementing DRM importing capability).
 */

/**
 * The intention of global namespace is to express general concept of placement
 * like system memory regardless of the underlying subsystem.  A device with
 * global system memory placement constraint is supposed to accept all system
 * memory backed buffers 
 */
#define DRM_BUFFER_CONSTRAINT_PLACEMENT_NAMESPACE_GLOBAL             1
#define   DRM_BUFFER_CONSTRAINT_PLACEMENT_ID_GLOBAL_SYSTEM             0
#define   DRM_BUFFER_CONSTRAINT_PLACEMENT_ID_GLOBAL_SYSTEM_CONTIGUOUS  1
#define   DRM_BUFFER_CONSTRAINT_PLACEMENT_ID_GLOBAL_PCI                2

#define DRM_BUFFER_CONSTRAINT_PLACEMENT_NAMESPACE_DMABUF_HEAP        2

#define DRM_BUFFER_CONSTRAINT_PLACEMENT_NAMESPACE_DRM                3
/* ID 0 for DRM sub-system is granted the sematics of local memory in the
 * context of queried device. */
#define   DRM_BUFFER_CONSTRAINT_PLACEMENT_ID_DRM_LOCAL                 0

#define DRM_BUFFER_CONSTRAINT_PLACEMENT_ATTRIBUTE_SYSTEM             (1 << 0)
#define DRM_BUFFER_CONSTRAINT_PLACEMENT_ATTRIBUTE_CONTIGUOUS         (1 << 1)
#define DRM_BUFFER_CONSTRAINT_PLACEMENT_ATTRIBUTE_DEVICE             (1 << 3)

struct drm_buffer_constraint_placement {
	/* Buffer placement could be sub-system specific.  For example,
	 * buffers in graphics world can be allocated via either DRM
	 * GEM API or DMA-BUF heap API which is widely used on mobile
	 * platform. */
	__u32 namespace;
	__u32 placement_id;
	/* For devices with multiple local memory banks, zero if not care. */
	__u32 bank;
	/* For placement in a subsystem-specific namespace,  this contains
	 * additional information for cooperation with namespace in global
	 * namespace. */
	__u32 attribute;
};

struct drm_buffer_constraint_data {
	/* Size of values in bytes. */
	__u32 size;
	__u32 values[0];
};

#define DRM_BUFFER_CONSTRAINT_TYPE_U32  1
#define DRM_BUFFER_CONSTRAINT_TYPE_U64  2
#define DRM_BUFFER_CONSTRAINT_TYPE_BOOL 3
#define DRM_BUFFER_CONSTRAINT_TYPE_BLOB 4

/**
 * Some constraints are as simple as a single integer whilst some are 
 * complated.
 */
struct drm_buffer_constraint {
	__u32 id;
	__u32 type;
	union {
		__u32 value32b;
		__u64 value64b;
		struct drm_buffer_constraint_data *data;
	} u;
};

struct drm_buffer_constraint_set {
	__u32 count;
	struct drm_buffer_constraint list[0];
};

#define DRM_BUFFER_CONSTRAINT_QUERY_ATOM_ID_FORMAT   1 /* type: u32 */
#define DRM_BUFFER_CONSTRAINT_QUERY_ATOM_ID_MODIFIER 2 /* type: u64 */
#define DRM_BUFFER_CONSTRAINT_QUERY_ATOM_ID_WIDTH    3 /* type: u32 */
#define DRM_BUFFER_CONSTRAINT_QUERY_ATOM_ID_HEIGHT   4 /* type: u32 */

#define DRM_BUFFER_CONSTRAINT_QUERY_ATOM_TYPE_U32  1
#define DRM_BUFFER_CONSTRAINT_QUERY_ATOM_TYPE_U64  2
#define DRM_BUFFER_CONSTRAINT_QUERY_ATOM_TYPE_BLOB 3

struct drm_buffer_constraint_query_atom {
	__u32 id;
	__u32 type;
	union {
		__u32 value32b;
		__u64 value64b;
		void *data;
	} u;
};

#define DRM_BUFFER_CONSTRAINT_TARGET_RENDER   (1 << 1)
#define DRM_BUFFER_CONSTRAINT_TARGET_SCANOUT  (1 << 2)
#define DRM_BUFFER_CONSTRAINT_TARGET_TEXTURE  (1 << 3)

/* Use pattern of the buffer, intended for performance optimization. */
#define DRM_BUFFER_CONSTRAINT_USAGE_CPU_READ  (1 << 1)
#define DRM_BUFFER_CONSTRAINT_USAGE_CPU_WRITE (1 << 2)

/**
 * @brief Ask for acceptable/recognized atoms to provide when querying
 * constraints.
 * 
 */
struct drm_buffer_constraint_prepare {
	__u32 target;
	__u32 usage;

	/* Following are returned by kernel. */
	__u32 count;
	__u32 *atom_ids;
};

struct drm_buffer_constraint_request {
	__u32 size;
	__u32 target;
	__u32 usage;
	__u32 atom_count;
	/**
	 * @list: A list of atoms (objects) providing additional info for
	 * determination of constraints.
	 */
	struct drm_buffer_constraint_query_atom *list;

	/* Following are returned by kernel. */
	struct drm_buffer_constraint_set *constraint_set;
};

#endif /* DRM_BUFFER_CONSTRAINT */