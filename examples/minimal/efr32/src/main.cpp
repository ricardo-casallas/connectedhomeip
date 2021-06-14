/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <AppConfig.h>
#include <AppTask.h>
#include <init_efrPlatform.h>

#include <lib/core/CHIPCore.h>
#include <support/CHIPMem.h>
#include <support/CHIPPlatformMemory.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/KeyValueStoreManager.h>
#include <platform/EFR32/Logging.h>

#include <mbedtls/threading.h>
#include <mbedtls/platform.h>

#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef HEAP_MONITORING
#include "MemMonitoring.h"
#endif

#if CHIP_ENABLE_OPENTHREAD
#include <mbedtls/platform.h>
#include <openthread/cli.h>
#include <openthread/dataset.h>
#include <openthread/error.h>
#include <openthread/heap.h>
#include <openthread/icmp6.h>
#include <openthread/instance.h>
#include <openthread/link.h>
#include <openthread/platform/openthread-system.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#endif // CHIP_ENABLE_OPENTHREAD

using namespace ::chip;
using namespace ::chip::DeviceLayer;

// ================================================================================
// Supporting functions
// ================================================================================

void appError(int err)
{
    EFR32_LOG("!!!!!!!!!!!! App Critical Error: %d !!!!!!!!!!!", err);
    portDISABLE_INTERRUPTS();
    while (1)
        ;
}

extern "C" void vApplicationIdleHook(void)
{
    // FreeRTOS Idle callback

    // Check CHIP Config nvm3 and repack flash if necessary.
    Internal::EFR32Config::RepackNvm3Flash();
}


// ================================================================================
// Main Code
// ================================================================================

int main(void)
{
    int ret = CHIP_ERROR_MAX;

    init_efrPlatform();

#ifdef HEAP_MONITORING
    MemMonitoring::startHeapMonitoring();
#endif

    mbedtls_platform_set_calloc_free(CHIPPlatformMemoryCalloc, CHIPPlatformMemoryFree);

    // Initialize mbedtls threading support on EFR32
    THREADING_setup();

    EFR32_LOG("==================================================");
    EFR32_LOG("chip-example-mini-efr32 starting 1.0.0");
    EFR32_LOG("==================================================");

    EFR32_LOG("Init CHIP Stack");

    // Init Chip memory management before the stack
    chip::Platform::MemoryInit();
    chip::DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().Init();

    ret = PlatformMgr().InitChipStack();
    if (ret != CHIP_NO_ERROR)
    {
        EFR32_LOG("PlatformMgr().InitChipStack() failed");
        appError(ret);
    }
    chip::DeviceLayer::ConnectivityMgr().SetBLEDeviceName("EFR32_MINI");

#if CHIP_ENABLE_OPENTHREAD
    EFR32_LOG("Initializing OpenThread stack");
    ret = ThreadStackMgr().InitThreadStack();
    if (ret != CHIP_NO_ERROR)
    {
        EFR32_LOG("ThreadStackMgr().InitThreadStack() failed");
        appError(ret);
    }

    ret = ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_Router);
    if (ret != CHIP_NO_ERROR)
    {
        EFR32_LOG("ConnectivityMgr().SetThreadDeviceType() failed");
        appError(ret);
    }

    EFR32_LOG("Starting OpenThread task");
    // Start OpenThread task
    ret = ThreadStackMgrImpl().StartThreadTask();
    if (ret != CHIP_NO_ERROR)
    {
        EFR32_LOG("ThreadStackMgr().StartThreadTask() failed");
        appError(ret);
    }

#endif // CHIP_ENABLE_OPENTHREAD

    EFR32_LOG("Starting Platform Manager Event Loop");
    ret = PlatformMgr().StartEventLoopTask();
    if (ret != CHIP_NO_ERROR)
    {
        EFR32_LOG("PlatformMgr().StartEventLoopTask() failed");
        appError(ret);
    }

    EFR32_LOG("Starting App Task");
    ret = AppTask::Instance().Init();
    if (ret != CHIP_NO_ERROR)
    {
        EFR32_LOG("GetAppTask().Init() failed");
        appError(ret);
    }

    EFR32_LOG("Starting FreeRTOS scheduler");
    vTaskStartScheduler();

}
