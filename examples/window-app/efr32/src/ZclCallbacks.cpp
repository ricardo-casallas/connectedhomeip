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

/**
 * @file
 *   This file implements the handler for data model messages.
 */

#include <WindowAppEFR32.h>
#include <AppConfig.h>
#include <app/Command.h>
#include <app/common/gen/attribute-id.h>
#include <app/common/gen/cluster-id.h>
#include <app/util/af-types.h>
#include <app/util/af.h>
#include <app/clusters/window-covering-server/window-covering-server.h>

using namespace ::chip;
using namespace example::efr32;

void emberAfPostAttributeChangeCallback(EndpointId endpoint, ClusterId clusterId, AttributeId attributeId, uint8_t mask,
                                        uint16_t manufacturerCode, uint8_t type, uint16_t size, uint8_t * value)
{
    EFR32_LOG("ATTRIBUTE CHANGE, ep:%u, cluster:%04x, attr:%04x, type:%02x", endpoint, clusterId, attributeId, type);

    if(ZCL_WINDOW_COVERING_CLUSTER_ID == clusterId) {
        WindowApp &app = WindowApp::Instance();
        WindowCover &cover = WindowCover::Instance();
        uint16_t current;
        uint16_t target;

        switch(attributeId)
        {
        case ZCL_WC_TYPE_ATTRIBUTE_ID:
            app.PostEvent(example::WindowEvent::CoverTypeChange);
            break;

        case ZCL_WC_CURRENT_POSITION_LIFT_PERCENT100_THS_ATTRIBUTE_ID:
            app.PostEvent(example::WindowEvent::CoverLiftChanged);
            break;

        case ZCL_WC_CURRENT_POSITION_TILT_PERCENT100_THS_ATTRIBUTE_ID:
            app.PostEvent(example::WindowEvent::CoverTiltChanged);
            break;

        case ZCL_WC_TARGET_POSITION_LIFT_PERCENT100_THS_ATTRIBUTE_ID:
            current = cover.Lift().PositionGet(WindowActuator::PositionUnits::Percentage100ths);
            target = cover.Lift().TargetGet(WindowActuator::PositionUnits::Percentage100ths);
            if(current > target) {
                app.PostEvent(example::WindowEvent::CoverLiftDown);
            }
            else if(current < target) {
                app.PostEvent(example::WindowEvent::CoverLiftUp);
            }
            break;

        case ZCL_WC_TARGET_POSITION_TILT_PERCENT100_THS_ATTRIBUTE_ID:
            current = cover.Tilt().PositionGet(WindowActuator::PositionUnits::Percentage100ths);
            target = cover.Tilt().TargetGet(WindowActuator::PositionUnits::Percentage100ths);
            if(current > target) {
                app.PostEvent(example::WindowEvent::CoverTiltDown);
            }
            else if(current < target) {
                app.PostEvent(example::WindowEvent::CoverTiltUp);
            }
            break;

        default:
            break;
        }
    }
    else {
        EFR32_LOG("Unknown cluster ID: %d", clusterId);
    }
}
