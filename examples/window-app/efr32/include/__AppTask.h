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

#include <WindowApp.h>
#include <AppTimer.h>
#include <LEDWidget.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <string>



#include <gpiointerrupt.h>
#include <hal-config-board.h>


namespace sample::window
{

struct Timer : public sample::Timer
{
    const char * mTag         = "*";
    TimerHandle_t mHandler    = nullptr;

    Timer(const char *name, uint32_t timeoutInMs, Callback callback, void *context);
    void IsrStart();
    virtual void Start();
    virtual void Stop();

private:
    static void TimerCallback(TimerHandle_t xTimer);
};


class AppTask: public WindowApp {

public:
    struct Button
    {
        struct ButtonConfig
        {
            GPIO_Port_TypeDef port;
            unsigned int pin;
        };

        Button(const char * name, Timer::Callback callback);

        void Init(const ButtonConfig &config);

        // ButtonId mId;
        // const char * mName = nullptr;
        GPIO_Port_TypeDef mPort;
        unsigned int mPin;
        bool mIsPressed = false;
        Timer mTimer;

        static void OnInterrupt(uint8_t pin);
        static void OnUpTimeout(sample::Timer & timer);
        static void OnDownTimeout(sample::Timer & timer);
    };

    static AppTask & Instance();
    static void Run(void * parameter);

    AppTask();
    virtual ~AppTask() {}

    virtual CHIP_ERROR Init();
    void ProcessEvents();
    virtual void PostEvent(const AppEvent & event);
    virtual sample::Timer *CreateTimer(const char *name, uint32_t timeoutInMs, sample::Timer::Callback callback, void *context);
    virtual void DestroyTimer(sample::Timer *timer);

protected:
    virtual void OnMainLoop();
    virtual void OnProvisionedStateChanged(bool isProvisioned);
    virtual void OnConnectivityStateChanged(bool isConnected);
    virtual void OnBLEConnectionsChanged(bool haveConnections);
    virtual void OnCoverTypeChanged();
    virtual void OnLiftChanged();
    virtual void OnTiltChanged();
    virtual void OnTiltModeChanged();
    virtual void OnResetWarning();
    virtual void OnResetCanceled();
    virtual void OnReset();

private:
    static AppTask sInstance;

    TaskHandle_t mHandle          = nullptr;
    QueueHandle_t mQueue = nullptr;
    Button mButtonUp;
    Button mButtonDown;
    LEDWidget mStatusLED;
    LEDWidget mActionLED;
    std::string mQRCode;
    Timer mIconTimer;
    
    static void OnIconTimeout(sample::Timer & timer);

    void UpdateLCD();
    void UpdateLEDs();
};

}
