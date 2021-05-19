/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

#include <shell/shell_core.h>
#include <shell/streamer.h>
#include <ChipShellCollection.h>
#include <messaging/ExchangeMgr.h>

#include <ChipShellCollection.h>
#include <platform/CHIPDeviceLayer.h>
#include <protocols/echo/Echo.h>
#include <protocols/secure_channel/PASESession.h>
#include <protocols/secure_channel/MessageCounterManager.h>
#include <shell/shell_core.h>
#include <support/ErrorStr.h>
#include <transport/SecureSessionMgr.h>
#include <transport/SecureSession.h>
#include <transport/raw/TCP.h>
#include <transport/raw/UDP.h>

// using namespace chip;
// using namespace Shell;
// using namespace Logging;
// using chip::Inet::IPAddress;
using chip::Shell::shell_command_t;
using chip::Shell::streamer_get;
using chip::Shell::streamer_printf;
using chip::Transport::TcpListenParameters;
using chip::Transport::UdpListenParameters;


#include <stdlib.h>
// #include <argp.h>
#include <string.h>


class ServerCommand
{
public:
    // enum struct Actions {
    //     None = 0,
    //     Start = 1,
    //     Stop = 2
    // }

    virtual ~ServerCommand() = default;
    virtual CHIP_ERROR Start() = 0;
    virtual CHIP_ERROR Stop() = 0;

};


class EchoCommand: public ServerCommand {

public:
    EchoCommand() = default;
    virtual ~EchoCommand() = default;

    CHIP_ERROR Start()
    {
        EFR32_LOG("#### EchoCommand.Start.0");

        if (mConnected)
        {
            return CHIP_ERROR_INCORRECT_STATE;
        }
        EFR32_LOG("#### EchoCommand.Start.1");
        chip::Optional<chip::Transport::PeerAddress> peer(chip::Transport::Type::kUndefined);
        EFR32_LOG("#### EchoCommand.Start.2");
        mAdminInfo = mAdmins.AssignAdminId(kAdminId, chip::kTestDeviceNodeId);
        EFR32_LOG("#### EchoCommand.Start.3");
        if (mAdminInfo == NULL)
        {
            return CHIP_ERROR_NO_MEMORY;
        }
        EFR32_LOG("#### EchoCommand.Start.4");
        CHIP_ERROR err = this->StartServer(peer);
        EFR32_LOG("#### EchoCommand.Start.5");
        return err;
    }

    CHIP_ERROR Stop()
    {
        EFR32_LOG("#### EchoCommand.Stop.0");
        this->StopServer();
        mEchoServer.Shutdown();
        mExchangeManager.Shutdown();
        mSessionManager.Shutdown();
        mAdmins.ReleaseAdminId(kAdminId);
        mAdmins.Reset();
        mConnected = false;
        EFR32_LOG("#### EchoCommand.Start.x");
        return CHIP_NO_ERROR;
    }

protected:

    virtual CHIP_ERROR StartServer(chip::Optional<chip::Transport::PeerAddress> &peer) = 0;
    virtual void StopServer() = 0;

    static void HandleEchoRequestReceived(chip::Messaging::ExchangeContext *ec, chip::System::PacketBufferHandle payload)
    {
        streamer_printf(streamer_get(), "Echo Request, len=%u ... sending response.\n", payload->DataLength());
    }

    bool mConnected = false;
    chip::Protocols::Echo::EchoServer mEchoServer;
    chip::SecureSessionMgr mSessionManager;
    chip::Messaging::ExchangeManager mExchangeManager;
    chip::Transport::AdminPairingTable mAdmins;
    chip::Transport::AdminPairingInfo * mAdminInfo = nullptr;
    static constexpr chip::Transport::AdminId kAdminId = 0;
    chip::SecurePairingUsingTestSecret mTestPairing;

    chip::secure_channel::MessageCounterManager mMessageCounterManager;
};


// #ifdef ARGP_LIB
// // const char *argp_program_version = "argp-ex3 1.0";
// // const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

// /* Program documentation. */
// static char doc[] = "CHIP Shell Server";

// /* A description of the arguments we accept. */
// static char args_doc[] = "<start/stop> <echo>";

// /* The options we understand. */
// static struct argp_option options[] = {
//   {"tcp",    't', 0,      0,  "TCP" },
//   { 0 }
// };

// /* Used by main to communicate with parse_opt. */
// struct arguments
// {
//     ServerAction action;
//     ServerCommand command;
//     bool tcp;
// };

// /* Parse a single option. */
// static error_t parse_opt (int key, char *arg, struct argp_state *state)
// {
// /* Get the input argument from argp_parse, which we
// know is a pointer to our arguments structure. */
//     struct arguments *arguments = (struct arguments *)state->input;
//     error_t err = 0;

//     switch (key)
//     {
//     case 't':
//         arguments->tcp = true;
//         break;

//     case ARGP_KEY_ARG:
//         if (state->arg_num >= 2)
//         {
//             /* Too many arguments. */
//             argp_usage(state);
//             err = EINVAL;
//         }
//         else if(0 == state->arg_num && 0 == strncmp("start", arg, 16))
//         {
//             arguments->action = ServerCommand::Actions::Start;
//         }
//         else if(0 == state->arg_num && 0 == strncmp("stop", arg, 16))
//         {
//             arguments->action = ServerCommand::Actions::Stop;
//         }
//         else if(1 == state->arg_num && 0 == strncmp("echo", arg, 16))
//         {
//             arguments->command = ServerCommand::Ids::Echo;
//         }
//         else
//         {
//             argp_usage(state);
//             err = EINVAL;
//         }
//         break;

//         case ARGP_KEY_END:
//         if (state->arg_num < 2)
//         {
//             /* Not enough arguments. */
//             argp_usage (state);
//             err = EINVAL;
//         }
//         break;

//     default:
//         err = ARGP_ERR_UNKNOWN;
//     }
//     return err;
// }

// /* Our argp parser. */
// static struct argp argp = { options, parse_opt, args_doc, doc };

// int cmd_server(int argc, char ** argv)
// {
//     struct arguments arguments;
//     CHIP_ERROR err = CHIP_NO_ERROR;
//     // error_t err = 0;

//     /* Default values. */
//     arguments.action = ServerAction::None;
//     arguments.command = ServerCommand::None;
//     arguments.tcp = false;

//     /* Parse our arguments; every option seen by parse_opt will
//     be reflected in arguments. */
//     err = argp_parse(&argp, argc, argv, ARGP_NO_EXIT, 0, &arguments);
//     if(0 == err)
//     {
//         if(ServerCommand::Ids::Echo == arguments.command)
//         {
//             EchoCommand echo;

//             if (ServerCommand::Actions::Start == arguments.action)
//             {
//                 err = echo.Start(arguments.tcp);
//             }
//             else
//             {
//                 err = echo.Stop();
//             }
//         }
//     }

//     return err;
// }
// #endif

#if INET_CONFIG_ENABLE_TCP_ENDPOINT

#if INET_CONFIG_ENABLE_IPV4

// TCP IPv4
class TcpEchoCommand: public EchoCommand {

public:
    using EchoCommand::EchoCommand;

protected:
    CHIP_ERROR StartServer(chip::Optional<chip::Transport::PeerAddress> &peer)
    {
        // TCP Manager
        ReturnErrorOnFailure(mTCPManager.Init(TcpListenParameters(&chip::DeviceLayer::InetLayer).SetAddressType(chip::Inet::kIPAddressType_IPv4),
                                              TcpListenParameters(&chip::DeviceLayer::InetLayer).SetAddressType(chip::Inet::kIPAddressType_IPv6)));
        // Session Manager
        ReturnErrorOnFailure(mSessionManager.Init(chip::kTestDeviceNodeId, &chip::DeviceLayer::SystemLayer, &mTCPManager, &mAdmins, &mMessageCounterManager));
        // Exchange Manager
        ReturnErrorOnFailure(mExchangeManager.Init(&mSessionManager));
        // Echo Server
        ReturnErrorOnFailure(mEchoServer.Init(&mExchangeManager));
        mEchoServer.SetEchoRequestReceived(EchoCommand::HandleEchoRequestReceived);
        // Pairing
        ReturnErrorOnFailure(mSessionManager.NewPairing(peer, chip::kTestControllerNodeId, &mTestPairing, chip::SecureSession::SessionRole::kInitiator, kAdminId));
        mConnected = true;
        return CHIP_NO_ERROR;
    }

    void StopServer()
    {
        mTCPManager.Close();
    }

private:
    static constexpr size_t kMaxTcpActiveConnectionCount = 4;
    static constexpr size_t kMaxTcpPendingPackets        = 4;

    chip::TransportMgr<chip::Transport::TCP<kMaxTcpActiveConnectionCount, kMaxTcpPendingPackets>,
                       chip::Transport::TCP<kMaxTcpActiveConnectionCount, kMaxTcpPendingPackets>> mTCPManager;
};

#else // INET_CONFIG_ENABLE_IPV4

// TCP IPv6
class TcpEchoCommand: public EchoCommand {
public:
    using EchoCommand::EchoCommand;

protected:

    CHIP_ERROR StartServer(chip::Optional<chip::Transport::PeerAddress> &peer)
    {
        // TCP Manager
        ReturnErrorOnFailure(mTCPManager.Init(TcpListenParameters(&chip::DeviceLayer::InetLayer).SetAddressType(chip::Inet::kIPAddressType_IPv6)));
        // Session Manager
        ReturnErrorOnFailure(mSessionManager.Init(chip::kTestDeviceNodeId, &chip::DeviceLayer::SystemLayer, &mTCPManager, &mAdmins, &mMessageCounterManager));
        // Exchange Manager
        ReturnErrorOnFailure(mExchangeManager.Init(&mSessionManager));
        // Echo Server
        ReturnErrorOnFailure(mEchoServer.Init(&mExchangeManager));
        mEchoServer.SetEchoRequestReceived(EchoCommand::HandleEchoRequestReceived);
        // Pairing
        ReturnErrorOnFailure(mSessionManager.NewPairing(peer, chip::kTestControllerNodeId, &mTestPairing, chip::SecureSession::SessionRole::kInitiator, kAdminId));
        mConnected = true;
        return CHIP_NO_ERROR;
    }

    void StopServer()
    {
        mTCPManager.Close();
    }

private:
    static constexpr size_t kMaxTcpActiveConnectionCount = 4;
    static constexpr size_t kMaxTcpPendingPackets        = 4;

    chip::TransportMgr<chip::Transport::TCP<kMaxTcpActiveConnectionCount, kMaxTcpPendingPackets>> mTCPManager;
};

#endif // INET_CONFIG_ENABLE_IPV4

#endif // INET_CONFIG_ENABLE_TCP_ENDPOINT

#if INET_CONFIG_ENABLE_IPV4

// UDP IPv4
class UdpEchoCommand: public EchoCommand {

public:
    using EchoCommand::EchoCommand;

protected:
    CHIP_ERROR StartServer(chip::Optional<chip::Transport::PeerAddress> &peer)
    {
        // UDP Manager
        ReturnErrorOnFailure(mUDPManager.Init(
            UdpListenParameters(&chip::DeviceLayer::InetLayer).SetAddressType(chip::Inet::kIPAddressType_IPv4),
            UdpListenParameters(&chip::DeviceLayer::InetLayer).SetAddressType(chip::Inet::kIPAddressType_IPv6)));
        // Session Manager
        ReturnErrorOnFailure(mSessionManager.Init(chip::kTestDeviceNodeId, &chip::DeviceLayer::SystemLayer, &mUDPManager, &mAdmins, &mMessageCounterManager));
        // Exchange Manager
        ReturnErrorOnFailure(mExchangeManager.Init(&mSessionManager));
        // Echo Server
        ReturnErrorOnFailure(mEchoServer.Init(&mExchangeManager));
        mEchoServer.SetEchoRequestReceived(EchoCommand::HandleEchoRequestReceived);
        // Pairing
        ReturnErrorOnFailure(mSessionManager.NewPairing(peer, chip::kTestControllerNodeId, &mTestPairing, chip::SecureSession::SessionRole::kInitiator, kAdminId));
        mConnected = true;
        return CHIP_NO_ERROR;
    }

    void StopServer()
    {
        mUDPManager.Close();
    }

private:
    chip::TransportMgr<chip::Transport::UDP, chip::Transport::UDP> mUDPManager;
};

#else // INET_CONFIG_ENABLE_IPV4

// UDP IPv6
class UdpEchoCommand: public EchoCommand {

public:
    using EchoCommand::EchoCommand;

protected:
    CHIP_ERROR StartServer(chip::Optional<chip::Transport::PeerAddress> &peer)
    {
        // UDP Manager
        ReturnErrorOnFailure(mUDPManager.Init(UdpListenParameters(&chip::DeviceLayer::InetLayer).SetAddressType(chip::Inet::kIPAddressType_IPv6)));
        // Session Manager
        // ReturnErrorOnFailure(mSessionManager.Init(kTestControllerNodeId, &chip::DeviceLayer::SystemLayer, &gUDPManager, &admins, &gMessageCounterManager));
        ReturnErrorOnFailure(mSessionManager.Init(chip::kTestDeviceNodeId, &chip::DeviceLayer::SystemLayer, &mUDPManager, &mAdmins, &mMessageCounterManager));
        // Exchange Manager
        ReturnErrorOnFailure(mExchangeManager.Init(&mSessionManager));
        // Echo Server
        ReturnErrorOnFailure(mEchoServer.Init(&mExchangeManager));
        mEchoServer.SetEchoRequestReceived(EchoCommand::HandleEchoRequestReceived);
        // Pairing
        ReturnErrorOnFailure(mSessionManager.NewPairing(peer, chip::kTestControllerNodeId, &mTestPairing, chip::SecureSession::SessionRole::kInitiator, kAdminId));
        
        mConnected = true;
        return CHIP_NO_ERROR;
    }

    void StopServer()
    {
        mUDPManager.Close();
    }
private:
    chip::TransportMgr<chip::Transport::UDP> mUDPManager;
};

#endif // INET_CONFIG_ENABLE_IPV4


// static int execute_command(ServerCommand &cmmd, const char *action) {
//     CHIP_ERROR err = CHIP_ERROR_INVALID_ARGUMENT;

//     return err;
// }

int cmd_server(int argc, char **argv)
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    bool tcp = false;
    int c;

    EFR32_LOG("#### cmd_server.0.1");

    // Parse options
    while((c = getopt(argc, argv, "t")) != -1)
    {
        printf("OPTION: '%c'\n", c);
        switch (c)
        {
        case 't':
            tcp = true;
            break;
        default:
            return CHIP_ERROR_INVALID_ARGUMENT;
        }
    }

    EFR32_LOG("#### cmd_server.1, argc:%d, optind:%d, cmd:'%s'", argc, optind, argv[optind]);

    // Parse arguments
    if(optind > 0 && (2 == argc) && 0 == strncmp("echo", argv[optind - 1], 16))
    {
        EchoCommand *echo = nullptr;

        if(tcp) {
            #if INET_CONFIG_ENABLE_TCP_ENDPOINT
            EFR32_LOG("#### cmd_server.2.1");
            echo = new TcpEchoCommand();
            #else
            EFR32_LOG("#### cmd_server.2.2");
            return CHIP_ERROR_INVALID_ARGUMENT;
            #endif
        }
        else {
            EFR32_LOG("#### cmd_server.3");
            echo = new UdpEchoCommand();
        }

        EFR32_LOG("#### cmd_server.4");
        if(0 == strncmp("start", argv[optind + 1], 16))
        {
            EFR32_LOG("#### cmd_server.5");
            err = echo->Start();
        }
        else if(0 == strncmp("stop", argv[optind + 1], 16))
        {
            EFR32_LOG("#### cmd_server.6");
            err = echo->Stop();
        }
    }
    else
    {
        EFR32_LOG("#### cmd_server.7");
        err = CHIP_ERROR_INVALID_ARGUMENT;
    }

    EFR32_LOG("#### cmd_server.8, err:%d", err);
    return err;
}

static shell_command_t cmds_server[] = {
    { &cmd_server, "server", "Setup CHIP protocol server" },
};

extern "C" void cmd_server_init()
{
    shell_register(cmds_server, 1);
}
