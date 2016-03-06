
/*
 * Copyright (c) 2012 NVIDIA Corporation.  All rights reserved.
 *
 * NVIDIA Corporation and its licensors retain all intellectual property and proprietary
 * rights in and to this software, related documentation and any modifications thereto.
 * Any use, reproduction, disclosure or distribution of this software and related
 * documentation without an express license agreement from NVIDIA Corporation is strictly
 * prohibited.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED *AS IS*
 * AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS BE LIABLE FOR ANY
 * SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT
 * LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE USE OF OR
 * INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES
 */
 
 /**
 * @file   optix_cuda_interop.h
 * @author NVIDIA Corporation
 * @brief  OptiX public API declarations CUDAInterop
 *
 * OptiX public API declarations for CUDA interoperability
 */

#ifndef __optix_optix_cuda_interop_h__
#define __optix_optix_cuda_interop_h__

#include "optix.h"

#if defined(__x86_64) || defined(AMD64) || defined(_M_AMD64)
typedef unsigned long long CUdeviceptr;
#else
typedef unsigned int CUdeviceptr;
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /**
  * @brief Creates a new buffer object that will later rely on user-side CUDA allocation
  * 
  * @ingroup Buffer
  * 
  * <B>Description</B>
  * 
  * @ref rtBufferCreateForCUDA allocates and returns a new handle to a new buffer object in *\a buffer
  * associated with \a context. This buffer will function like a normal OptiX buffer created with @ref rtBufferCreate,
  * except OptiX will not allocate or upload data for it.
  *
  * After a buffer object has been created with @ref rtBufferCreateForCUDA, the user needs to call
  * @ref rtBufferSetDevicePointer to provide one or more device pointers to the buffer data.
  * When the user provides a single device's data pointer for a buffer prior to calling @ref rtContextLaunch "rtContextLaunch",
  * OptiX will allocate memory on the other devices and copy the data there. Setting pointers for more than one
  * but fewer than all devices is not supported.
  *
  * If @ref rtBufferSetDevicePointer or @ref rtBufferGetDevicePointer have been called for a single device for a given buffer,
  * the user can change the buffer's content on that device. OptiX must then synchronize the new buffer contents to all devices.
  * These synchronization copies occur at every @ref rtContextLaunch "rtContextLaunch", unless the buffer is declared with @ref RT_BUFFER_COPY_ON_DIRTY.
  * In this case, use @ref rtBufferMarkDirty to notify OptiX that the buffer has been dirtied and must be synchronized.
  *
  * The backing storage of the buffer is managed by OptiX. A buffer is specified by a bitwise 
  * \a or combination of a \a type and \a flags in \a bufferdesc. The supported types are:
  *
  * - @ref RT_BUFFER_INPUT
  * - @ref RT_BUFFER_OUTPUT
  * - @ref RT_BUFFER_INPUT_OUTPUT
  *
  * The type values are used to specify the direction of data flow from the host to the OptiX devices.
  * @ref RT_BUFFER_INPUT specifies that the host may only write to the buffer and the device may only read from the buffer.
  * @ref RT_BUFFER_OUTPUT specifies the opposite, read only access on the host and write only access on the device.
  * Devices and the host may read and write from buffers of type @ref RT_BUFFER_INPUT_OUTPUT. Reading or writing to
  * a buffer of the incorrect type (e.g., the host writing to a buffer of type @ref RT_BUFFER_OUTPUT) is undefined.
  *
  * The supported flags are:
  *
  * - @ref RT_BUFFER_GPU_LOCAL
  * - @ref RT_BUFFER_COPY_ON_DIRTY
  *
  * Flags can be used to optimize data transfers between the host and its devices. The flag @ref RT_BUFFER_GPU_LOCAL can only be 
  * used in combination with @ref RT_BUFFER_INPUT_OUTPUT. @ref RT_BUFFER_INPUT_OUTPUT and @ref RT_BUFFER_GPU_LOCAL used together specify a buffer 
  * that allows the host to \b only write, and the device to read \b and write data. The written data will be never visible 
  * on the host side.
  * 
  * @param[in]   context          The context to create the buffer in
  * @param[in]   bufferdesc       Bitwise \a or combination of the \a type and \a flags of the new buffer
  * @param[out]  buffer           The return handle for the buffer object
  * 
  * <B>Return values</B>
  *
  * Relevant return values:
  * - @ref RT_SUCCESS
  * - @ref RT_ERROR_INVALID_CONTEXT
  * - @ref RT_ERROR_INVALID_VALUE
  * - @ref RT_ERROR_MEMORY_ALLOCATION_FAILED
  * 
  * <B>History</B>
  * 
  * @ref rtBufferCreateForCUDA was introduced in OptiX 3.0.
  * 
  * <B>See also</B>
  * @ref rtBufferCreate,
  * @ref rtBufferSetDevicePointer,
  * @ref rtBufferMarkDirty,
  * @ref rtBufferDestroy
  * 
  */
  RTresult RTAPI rtBufferCreateForCUDA (RTcontext context, unsigned int bufferdesc, RTbuffer *buffer);
  
  /**
  * @brief Gets the pointer to the buffer's data on the given device
  * 
  * @ingroup Buffer
  * 
  * <B>Description</B>
  * 
  * @ref rtBufferGetDevicePointer returns the pointer to the data of \a buffer on device \a optix_device_number in **\a device_pointer.
  *
  * If @ref rtBufferGetDevicePointer has been called for a single device for a given buffer,
  * the user can change the buffer's content on that device. OptiX must then synchronize the new buffer contents to all devices.
  * These synchronization copies occur at every @ref rtContextLaunch "rtContextLaunch", unless the buffer is declared with @ref RT_BUFFER_COPY_ON_DIRTY.
  * In this case, use @ref rtBufferMarkDirty to notify OptiX that the buffer has been dirtied and must be synchronized.
  * 
  * @param[in]   buffer                          The buffer to be queried for its device pointer
  * @param[in]   optix_device_number             The number of OptiX device
  * @param[out]  device_pointer                  The return handle to the buffer's device pointer
  * 
  * <B>Return values</B>
  *
  * Relevant return values:
  * - @ref RT_SUCCESS
  * - @ref RT_ERROR_INVALID_CONTEXT
  * - @ref RT_ERROR_INVALID_VALUE
  * 
  * <B>History</B>
  * 
  * @ref rtBufferGetDevicePointer was introduced in OptiX 3.0.
  * 
  * <B>See also</B>
  * @ref rtBufferMarkDirty,
  * @ref rtBufferSetDevicePointer
  * 
  */
  RTresult RTAPI rtBufferGetDevicePointer (RTbuffer buffer, unsigned int optix_device_number, void** device_pointer);
  
  /**
  * @brief Sets a buffer as dirty
  * 
  * @ingroup Buffer
  * 
  * <B>Description</B>
  * 
  * If @ref rtBufferSetDevicePointer or @ref rtBufferGetDevicePointer have been called for a single device for a given buffer,
  * the user can change the buffer's content on that device. OptiX must then synchronize the new buffer contents to all devices.
  * These synchronization copies occur at every @ref rtContextLaunch "rtContextLaunch", unless the buffer is declared with @ref RT_BUFFER_COPY_ON_DIRTY.
  * In this case, use @ref rtBufferMarkDirty to notify OptiX that the buffer has been dirtied and must be synchronized.
  *
  * Note that RT_BUFFER_COPY_ON_DIRTY currently only applies to CUDA Interop buffers (buffers for which the application has a device pointer).
  * 
  * @param[in]   buffer                          The buffer to be marked dirty
  * 
  * <B>Return values</B>
  *
  * Relevant return values:
  * - @ref RT_SUCCESS
  * - @ref RT_ERROR_INVALID_VALUE
  * 
  * <B>History</B>
  * 
  * @ref rtBufferMarkDirty was introduced in OptiX 3.0.
  * 
  * <B>See also</B>
  * @ref rtBufferGetDevicePointer,
  * @ref rtBufferSetDevicePointer,
  * @ref RT_BUFFER_COPY_ON_DIRTY
  * 
  */
  RTresult RTAPI rtBufferMarkDirty (RTbuffer buffer);
  
  /**
  * @brief Sets the pointer to the buffer's data on the given device
  * 
  * @ingroup Buffer
  * 
  * <B>Description</B>
  * 
  * @ref rtBufferSetDevicePointer sets the pointer to the data of \a buffer on device \a optix_device_number to \a device_pointer.
  *
  * The buffer needs to be allocated with @ref rtBufferCreateForCUDA in order for the call to @ref rtBufferSetDevicePointer to be valid.
  * Likewise, before providing a device pointer for the buffer, the application must first specify the size and format of the buffer.
  *
  * If @ref rtBufferSetDevicePointer has been called for a single device for a given buffer,
  * the user can change the buffer's content on that device. OptiX must then synchronize the new buffer contents to all devices.
  * These synchronization copies occur at every @ref rtContextLaunch "rtContextLaunch", unless the buffer is declared with @ref RT_BUFFER_COPY_ON_DIRTY.
  * In this case, use @ref rtBufferMarkDirty to notify OptiX that the buffer has been dirtied and must be synchronized.
  * 
  * @param[in]   buffer                          The buffer for which the device pointer is to be set
  * @param[in]   optix_device_number             The number of OptiX device
  * @param[in]   device_pointer                  The pointer to the data on the specified device
  * 
  * <B>Return values</B>
  *
  * Relevant return values:
  * - @ref RT_SUCCESS
  * - @ref RT_ERROR_INVALID_VALUE
  * - @ref RT_ERROR_INVALID_CONTEXT
  * 
  * <B>History</B>
  * 
  * @ref rtBufferSetDevicePointer was introduced in OptiX 3.0.
  * 
  * <B>See also</B>
  * @ref rtBufferMarkDirty,
  * @ref rtBufferGetDevicePointer
  * 
  */
  RTresult RTAPI rtBufferSetDevicePointer (RTbuffer buffer, unsigned int optix_device_number, CUdeviceptr device_pointer);

#ifdef __cplusplus
}
#endif

#endif /* __optix_optix_cuda_interop_h__ */
