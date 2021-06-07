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

#pragma once

/*
WARNING: This file is included by other EFR32 sources, like
         src/platform/EFR32/freertos_bluetooth.c
*/

#define APP_TASK_NAME           "Minimal_Task"
#define APP_TASK_STACK_SIZE     4096
// #define APP_TASK_PRIORITY       3
#define APP_BUTTON_DEBOUNCE_PERIOD_MS 50
