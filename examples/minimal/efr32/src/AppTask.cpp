#include <AppTask.h>
#include <ButtonHandler.h>
#include <app/server/Server.h>
#include <platform/EFR32/Logging.h>
#include <lib/support/CodeUtils.h>
#include <platform/CHIPDeviceLayer.h>
#include <Service.h>


AppTask AppTask::sInstance;
uint8_t AppTask::sCount = 0;


AppTask & AppTask::Instance()
{
    return sInstance;
}


int AppTask::Init()
{
    mHandle = xTaskCreateStatic(AppTask::Main, APP_TASK_NAME, ArraySize(mStack), NULL, 1, mStack, &mStruct);
    if (!mHandle)
    {
        EFR32_LOG("AppTask initialization ERROR");
    }

    InitServer();

    // Device Configuration
    chip::DeviceLayer::ConfigurationMgr().LogDeviceConfig();
    SetDeviceName("EFR32WindowCoverDemo._chip._udp.local.");

    ButtonHandler::Instance().Init(AppTask::OnButton);

    return CHIP_NO_ERROR;
}


int AppTask::Start()
{
    Init();
    return CHIP_NO_ERROR;
}


void AppTask::Main(void *param)
{
    while (true)
    {
        sCount += 1;
        EFR32_LOG("Minimal running... %u", sCount);
        sleep(2);
        if (sCount > 99)
        {
            sCount = 0;
        }
    }
}


void AppTask::OnButton(ButtonHandler::Button &btn)
{
    // EFR32_LOG("BUTTON %u, on:%u", btn.mId, btn.mIsPressed);
    if(btn.mIsPressed && (0 == btn.mId))
    {
        EFR32_LOG("RESET!");
        chip::DeviceLayer::ConfigurationMgr().InitiateFactoryReset();
    }
}


unsigned int sleep(unsigned int seconds)
{
    const TickType_t xDelay = 1000 * seconds / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
    return 0;
}
