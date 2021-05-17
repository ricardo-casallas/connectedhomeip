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

// #include <platform/CHIPDeviceLayer.h>
// #include <platform/KeyValueStoreManager.h>
// #include <support/CHIPMem.h>
// #include <support/CHIPPlatformMemory.h>
#include <ChipShellCollection.h>
#include <lib/shell/shell_core.h>
#include <support/CHIPMem.h>
#include <lib/support/CHIPPlatformMemory.h>
#include <platform/CHIPDeviceLayer.h>

#include "AppConfig.h"
#include "init_efrPlatform.h"

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
static TaskHandle_t sShellTaskHandle;

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


int main(void)
{
    EFR32_LOG("==================================================");
    EFR32_LOG("chip-efr32-shell-example starting");
    EFR32_LOG("==================================================");

    EFR32_LOG("Init CHIP Stack");

    init_efrPlatform();
    // Initialize LEDs
    // LEDWidget::InitGpio();
    // sStatusLED.Init(SYSTEM_STATE_LED);
    // sStatusLED.Set(true);

    chip::Platform::MemoryInit();
    chip::DeviceLayer::PlatformMgr().InitChipStack();
    chip::DeviceLayer::PlatformMgr().StartEventLoopTask();
    int ret = chip::Shell::streamer_init(chip::Shell::streamer_get());
    assert(ret == 0);

    // cmd_misc_init();
    // cmd_base64_init();
    // cmd_device_init();
    // cmd_btp_init();
    cmd_otcli_init();
    cmd_ping_init();
    cmd_send_init();

    xTaskCreate(shell_task, "Shell_Task", SHELL_TASK_STACK_SIZE, nullptr, SHELL_TASK_PRIORITY, &sShellTaskHandle);

    vTaskStartScheduler();
}
