/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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

#include "Service.h"

#include "AppConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <sys/param.h>

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <inet/IPAddress.h>
#include <inet/InetError.h>
#include <inet/InetLayer.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/EFR32/Logging.h>
#include <support/CodeUtils.h>
#include <support/ErrorStr.h>
#include <system/SystemPacketBuffer.h>
#include <transport/SecureSessionMgr.h>
#include <transport/raw/UDP.h>


#if CHIP_ENABLE_OPENTHREAD
#include <openthread/message.h>
#include <openthread/udp.h>
#include <platform/EFR32/ThreadStackManagerImpl.h>
#include <platform/OpenThread/OpenThreadUtils.h>
#include <platform/ThreadStackManager.h>
#include <platform/internal/DeviceNetworkInfo.h>
#endif

using namespace ::chip;
using namespace ::chip::Inet;
using namespace ::chip::Transport;
using namespace ::chip::DeviceLayer;

static char sDeviceName[128] = { 0 };
// Hardcode UDP BroadcastPort. Temporary use for demo with OTBR
constexpr uint16_t kUDPBroadcastPort = 23367;

void SetDeviceName(const char * newDeviceName)
{
    strncpy(sDeviceName, newDeviceName, sizeof(sDeviceName) - 1);
}

void PublishService()
{
    chip::Inet::IPAddress addr;
    if (!ConnectivityMgrImpl().IsThreadAttached())
    {
        return;
    }
    ThreadStackMgrImpl().LockThreadStack();
    otError error = OT_ERROR_NONE;
    otMessageInfo messageInfo;
    otUdpSocket mSocket;
    otMessage * message = nullptr;

    memset(&mSocket, 0, sizeof(mSocket));
    memset(&messageInfo, 0, sizeof(messageInfo));

    // Use mesh local EID by default, if we have GUA, use that IP address.
    memcpy(&messageInfo.mSockAddr, otThreadGetMeshLocalEid(ThreadStackMgrImpl().OTInstance()), sizeof(messageInfo.mSockAddr));

    // Select a address to send
    const otNetifAddress * otAddrs = otIp6GetUnicastAddresses(ThreadStackMgrImpl().OTInstance());
    for (const otNetifAddress * otAddr = otAddrs; otAddr != NULL; otAddr = otAddr->mNext)
    {
        addr = chip::DeviceLayer::Internal::ToIPAddress(otAddr->mAddress);
        if (otAddr->mValid && addr.IsIPv6GlobalUnicast())
        {
            memcpy(&messageInfo.mSockAddr, &(otAddr->mAddress), sizeof(otAddr->mAddress));
            break;
        }
    }

    message = otUdpNewMessage(ThreadStackMgrImpl().OTInstance(), nullptr);
    otIp6AddressFromString("ff03::1", &messageInfo.mPeerAddr);
    messageInfo.mPeerPort = kUDPBroadcastPort;
    otMessageAppend(message, sDeviceName, static_cast<uint16_t>(strlen(sDeviceName)));

    error = otUdpSend(ThreadStackMgrImpl().OTInstance(), &mSocket, message, &messageInfo);

    if (error != OT_ERROR_NONE && message != nullptr)
    {
        otMessageFree(message);
        EFR32_LOG("Failed to otUdpSend: %d", error);
    }
    ThreadStackMgrImpl().UnlockThreadStack();
}
