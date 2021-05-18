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

#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <mbedtls/threading.h>

#include <platform/CHIPDeviceLayer.h>
#include <platform/KeyValueStoreManager.h>
#include <support/CHIPMem.h>
#include <support/CHIPPlatformMemory.h>

#include <ChipShellCollection.h>
#include <lib/shell/shell_core.h>
#include <AppConfig.h>
#include <init_efrPlatform.h>

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

using namespace chip;
using namespace chip::Shell;
using namespace ::chip::Inet;
using namespace ::chip::DeviceLayer;

#define SHELL_TASK_STACK_SIZE 8192
#define SHELL_TASK_PRIORITY 3

#define UNUSED_PARAMETER(a) (a = a)

volatile int apperror_cnt;
// ================================================================================
// App Error
//=================================================================================
void appError(int err)
{
    EFR32_LOG("!!!!!!!!!!!! App Critical Error: %d !!!!!!!!!!!", err);
    portDISABLE_INTERRUPTS();
    while (1)
        ;
}

///FIXME
unsigned int sleep(unsigned int seconds)
{
    return 0;
}

// ================================================================================
// FreeRTOS Callbacks
// ================================================================================
extern "C" void vApplicationIdleHook(void)
{
    // FreeRTOS Idle callback

    // Check CHIP Config nvm3 and repack flash if necessary.
    Internal::EFR32Config::RepackNvm3Flash();
}

// ================================================================================
// Main Code
// ================================================================================

#define APP_TASK_STACK_SIZE (1536)
static StackType_t appStack[APP_TASK_STACK_SIZE / sizeof(StackType_t)];
static StaticTask_t appTaskStruct;

int main(void)
{
    int err = CHIP_ERROR_MAX;

    init_efrPlatform();
    mbedtls_platform_set_calloc_free(CHIPPlatformMemoryCalloc, CHIPPlatformMemoryFree);

    // Initialize mbedtls threading support on EFR32
    THREADING_setup();

    EFR32_LOG("==================================================");
    EFR32_LOG("chip-efr32-shell-example starting 1.0.0");
    EFR32_LOG("==================================================");

    EFR32_LOG("Init CHIP Stack");

    // Init Chip memory management before the stack
    chip::Platform::MemoryInit();
    chip::DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().Init();

    err = PlatformMgr().InitChipStack();
    if (err != CHIP_NO_ERROR)
    {
        EFR32_LOG("PlatformMgr().InitChipStack() failed");
        appError(err);
    }

    EFR32_LOG("Starting Platform Manager Event Loop");
    err = PlatformMgr().StartEventLoopTask();
    if (err != CHIP_NO_ERROR)
    {
        EFR32_LOG("PlatformMgr().StartEventLoopTask() failed");
        appError(err);
    }

    err = chip::Shell::streamer_init(chip::Shell::streamer_get());
    assert(err == 0);

    // cmd_misc_init();
    // cmd_base64_init();
    // cmd_device_init(); // FIXME: Hard fault!
    // cmd_btp_init();
    cmd_otcli_init();
    cmd_ping_init();
    cmd_send_init();

    TaskHandle_t sAppTaskHandle = xTaskCreateStatic(shell_task, APP_TASK_NAME, ArraySize(appStack), NULL, 1, appStack, &appTaskStruct);
    if (! sAppTaskHandle)
    {
        EFR32_LOG("MEMORY ERROR!!!");
        err = CHIP_ERROR_NO_MEMORY;
    }

    vTaskStartScheduler();
    return err;
}
