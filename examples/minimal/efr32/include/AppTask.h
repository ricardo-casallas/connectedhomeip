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

#include "AppConfig.h"
#include <ButtonHandler.h>
#include <FreeRTOS.h>
#include <task.h>

class AppTask
{
public:
    static AppTask & Instance();
    int Start();

private:
    static AppTask sInstance;
    static uint8_t sCount;

    static void Main(void *param);
    static void OnButton(ButtonHandler::Button &btn);

    TaskHandle_t mHandle;
    StackType_t mStack[APP_TASK_STACK_SIZE / sizeof(StackType_t)];
    StaticTask_t mStruct;

    AppTask() = default;
    int Init();
};