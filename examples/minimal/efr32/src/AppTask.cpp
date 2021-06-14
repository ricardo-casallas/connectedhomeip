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
    // Device Configuration
    ButtonHandler::Instance().Init(AppTask::OnButton);

    mHandle = xTaskCreateStatic(AppTask::Main, APP_TASK_NAME, ArraySize(mStack), NULL, 1, mStack, &mStruct);
    if (!mHandle)
    {
        EFR32_LOG("AppTask initialization ERROR");
    }

    return CHIP_NO_ERROR;
}


void AppTask::Main(void *param)
{
    uint64_t lastChangeTimeUS = 0;
    uint64_t nextChangeTimeUS = 0;
    uint64_t nowUS = 0;
    
    InitServer();

    EFR32_LOG("Current Firmware Version: %s", CHIP_DEVICE_CONFIG_DEVICE_FIRMWARE_REVISION_STRING);
    chip::DeviceLayer::ConfigurationMgr().LogDeviceConfig();

    EFR32_LOG("App Task started");
    SetDeviceName("EFR32MinimalDemo._chip._udp.local.");

    while (true)
    {
        sCount += 1;
        if(0 == sCount % 20)
        {
            EFR32_LOG("Minimal running... %u", sCount);
        }
        sleep(1);
        if (sCount > 99)
        {
            sCount = 0;
        }

        nowUS = chip::System::Platform::Layer::GetClock_Monotonic();
        nextChangeTimeUS = lastChangeTimeUS + 5 * 1000 * 1000UL;
        if (nowUS > nextChangeTimeUS)
        {
            PublishService();
            lastChangeTimeUS = nowUS;
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
