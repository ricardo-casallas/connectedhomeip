/**
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
 *
 *    Copyright (c) 2020 Silicon Labs
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
/****************************************************************************
 * @file
 * @brief Routines for the Window Covering Server cluster
 *******************************************************************************
 ******************************************************************************/


#include <app/util/af.h>

#include <app/common/gen/attribute-id.h>
#include <app/common/gen/command-id.h>
#include <app/common/gen/cluster-id.h>


#include <app/Command.h>
#include <app/reporting/reporting.h>
#include <app/util/af-event.h>
#include <app/util/af-types.h>
#include <app/util/attribute-storage.h>

#ifdef EMBER_AF_PLUGIN_SCENES
#include <app/clusters/scenes/scenes.h>
#endif // EMBER_AF_PLUGIN_SCENES

#include "window-covering-server.h"
using namespace chip;

EmberAfStatus wcWriteAttribute(chip::AttributeId attributeID, uint8_t * dataPtr, EmberAfAttributeType dataType)
{
    if (!dataPtr) return EMBER_ZCL_STATUS_INVALID_FIELD;

    EmberAfStatus status = emberAfWriteAttribute(emberAfCurrentEndpoint(), ZCL_WINDOW_COVERING_CLUSTER_ID, attributeID, CLUSTER_MASK_SERVER, dataPtr, dataType);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
        emberAfWindowCoveringClusterPrint("Err: WC Writing Attribute failed: %x", status);
    }
    return status;
}


EmberAfStatus wcReadAttribute(chip::AttributeId attributeID, uint8_t * dataPtr, uint16_t readLength)
{
    if (!dataPtr) return EMBER_ZCL_STATUS_INVALID_FIELD;

    EmberAfStatus status = emberAfReadAttribute(emberAfCurrentEndpoint(), ZCL_WINDOW_COVERING_CLUSTER_ID, attributeID, CLUSTER_MASK_SERVER, dataPtr, readLength, NULL);
    if (status != EMBER_ZCL_STATUS_SUCCESS) {
        emberAfWindowCoveringClusterPrint("Err: WC Reading Attribute failed: %x", status);
    }

    return status;
}


//------------------------------------------------------------------------------
// WindowCover
//------------------------------------------------------------------------------

WindowCover WindowCover::sInstance;


WindowCover & WindowCover::Instance()
{
    return sInstance;
}


WindowCover::WindowCover()
{
}

void WindowCover::TypeSet(EmberAfWcType type)
{
    mType = type;
}


EmberAfWcType WindowCover::TypeGet(void)
{
    return mType; 
}


void WindowCover::ConfigStatusSet(ConfigStatus_t status)
{
    mConfigStatus = status;
}


ConfigStatus_t WindowCover::ConfigStatusGet(void)
{
    return mConfigStatus;
}


void WindowCover::OperationalStatusSet(OperationalStatus_t status)
{
    mOperationalStatus = status;
}


OperationalStatus_t WindowCover::OperationalStatusGet(void)
{
    return mOperationalStatus;
}


void WindowCover::EndProductTypeSet(EmberAfWcEndProductType type)
{
    mEndProductType = type;
}


EmberAfWcEndProductType WindowCover::EndProductTypeGet(void)
{
    return mEndProductType;
}


void WindowCover::ModeSet(Mode_t mode)
{
    mMode = mode;
}


Mode_t WindowCover::ModeGet(void)
{
    return mMode;
}

void WindowCover::SafetyStatusSet(uint16_t status)
{
    mSafetyStatus = status;
}


uint16_t WindowCover::SafetyStatusGet(void)
{
    return mSafetyStatus;
}

//------------------------------------------------------------------------------
// Actuator
//------------------------------------------------------------------------------

void WindowActuator::DeltaSet(uint16_t delta)
{
    mStep = delta;
}


void WindowActuator::OpenLimitSet(uint16_t limit)
{
    // EmberAfStatus status = 
    wcWriteAttribute(mAttributeIds.openLimit, (uint8_t *) &limit, ZCL_INT16U_ATTRIBUTE_TYPE);
}


uint16_t WindowActuator::OpenLimitGet(void)
{
    uint16_t limit = 0;
    // EmberAfStatus status = 
    wcReadAttribute(mAttributeIds.openLimit, (uint8_t *) &limit, sizeof(uint16_t));
    return limit;
}


void WindowActuator::ClosedLimitSet(uint16_t limit)
{
    // EmberAfStatus status = 
    wcWriteAttribute(mAttributeIds.closedLimit, (uint8_t *) &limit, ZCL_INT16U_ATTRIBUTE_TYPE);
}


uint16_t WindowActuator::ClosedLimitGet(void)
{
    uint16_t limit = 0;
    // EmberAfStatus status = 
    wcReadAttribute(mAttributeIds.closedLimit, (uint8_t *) &limit, sizeof(uint16_t));
    return limit;
}


void WindowActuator::PositionSet(uint16_t value, PositionUnits units)
{
    EmberAfStatus status = EMBER_ZCL_STATUS_ACTION_DENIED;
    uint8_t percent = 0;
    uint16_t percent100ths = 0;
    uint16_t position = 0;

    switch(units) {
        case PositionUnits::Value:
            position = value;
            percent100ths = ValueToPercent100ths(value);
            percent = percent100ths / 100;
            break;
        case PositionUnits::Percentage:
            percent = value;
            percent100ths = value * 100;
            position = Percent100thsToValue(percent100ths);
            break;
        case PositionUnits::Percentage100ths:
        default:
            percent100ths = value;
            percent = value / 100;
            position = Percent100thsToValue(percent100ths);
            break;
    }

    status = wcWriteAttribute(mAttributeIds.positionValue, (uint8_t *) &position, ZCL_INT16U_ATTRIBUTE_TYPE);
    if(EMBER_ZCL_STATUS_SUCCESS != status) {
        return;
    }

    status = wcWriteAttribute(mAttributeIds.positionPercent, (uint8_t *) &percent, ZCL_INT16U_ATTRIBUTE_TYPE);
    if(EMBER_ZCL_STATUS_SUCCESS != status) {
        return;
    }

    status = wcWriteAttribute(mAttributeIds.positionPercent100ths, (uint8_t *) &percent100ths, ZCL_INT16U_ATTRIBUTE_TYPE);
    if(EMBER_ZCL_STATUS_SUCCESS != status) {
        return;
    }
}


uint16_t WindowActuator::PositionGet(PositionUnits units)
{
    uint16_t percent100ths = 0;

    EmberAfStatus status = wcReadAttribute(ZCL_WC_CURRENT_POSITION_LIFT_PERCENT100_THS_ATTRIBUTE_ID, (uint8_t *) &percent100ths, sizeof(uint16_t));
    if(EMBER_ZCL_STATUS_SUCCESS != status) {
        return 0;
    }
    switch(units) {
    case PositionUnits::Value:
        return Percent100thsToValue(percent100ths);
    case PositionUnits::Percentage:
        return (uint16_t) (percent100ths / 100);
    case PositionUnits::Percentage100ths:
    default:
        return percent100ths;
    }
}



void WindowActuator::NumberOfActuationsIncrement()
{
}


uint8_t WindowActuator::NumberOfActuationsGet(void)
{
    return 0;
}


void WindowActuator::TargetSet(uint16_t value, PositionUnits units)
{
    switch(units) {
        case PositionUnits::Value:
            mTarget = ValueToPercent100ths(value);
            break;
        case PositionUnits::Percentage:
            mTarget = value * 100;
            break;
        case PositionUnits::Percentage100ths:
        default:
            mTarget = value;
            break;
    }
}


uint16_t WindowActuator::TargetGet(PositionUnits units)
{
    switch(units) {
        case PositionUnits::Value:
            return Percent100thsToValue(mTarget);
        case PositionUnits::Percentage:
            return (uint16_t) (mTarget / 100);
        case PositionUnits::Percentage100ths:
        default:
            return mTarget;
    }
}


uint16_t WindowActuator::ValueToPercent100ths(uint16_t position)
{
    uint16_t openLimit  = OpenLimitGet();
    uint16_t closedLimit = ClosedLimitGet();
    posPercent100ths_t minimum = 0, range = UINT16_MAX;

    if (openLimit > closedLimit) {
        minimum = closedLimit;
        range = openLimit - minimum;
    } else {
        minimum = openLimit;
        range = closedLimit - minimum;
    }

    if (position < minimum)
        return 0;

    if (range > 0) {
        return (posPercent100ths_t) (WC_PERCENT100THS_MAX * (position - minimum) / range);
    }

    return WC_PERCENT100THS_MAX;
}


uint16_t WindowActuator::Percent100thsToValue(posPercent100ths_t percent100ths)
{
    uint16_t openLimit  = OpenLimitGet();
    uint16_t closedLimit = ClosedLimitGet();
    uint16_t minimum = 0, maximum = UINT16_MAX, range = UINT16_MAX;

    if (openLimit > closedLimit) {
        minimum = closedLimit;
        maximum = openLimit;
    } else {
        minimum = openLimit;
        maximum = closedLimit;
    }

    range = maximum - minimum;

    if (percent100ths > WC_PERCENT100THS_MAX) return maximum;

    return minimum + ((range * percent100ths) / WC_PERCENT100THS_MAX);
}


//------------------------------------------------------------------------------
// Callbacks
//------------------------------------------------------------------------------


/** @brief Window Covering Cluster Init
 *
 * Cluster Init
 *
 * @param endpoint    Endpoint that is being initialized
 */
void emberAfWindowCoveringClusterInitCallback(chip::EndpointId endpoint)
{
    emberAfWindowCoveringClusterPrint("Window Covering Cluster init");
}


/**
 * @brief Window Covering Cluster UpOrOpen Command callback
 */
bool __attribute__((weak)) emberAfWindowCoveringClusterUpOrOpenCallback(chip::app::Command *cmmd)
{
    WindowCover &cover = WindowCover::Instance();
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("UpOrOpen command received");    
    if(cover.hasFeature(WindowCover::Feature::Lift)) {
        cover.Lift().TargetSet(WC_PERCENT100THS_MAX, WindowActuator::PositionUnits::Percentage100ths);
    }
    if(cover.hasFeature(WindowCover::Feature::Tilt)) {
        cover.Tilt().TargetSet(WC_PERCENT100THS_MAX, WindowActuator::PositionUnits::Percentage100ths);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}


/**
 * @brief Window Covering Cluster DownOrClose Command callback
 */
bool __attribute__((weak)) emberAfWindowCoveringClusterDownOrCloseCallback(chip::app::Command *cmmd)
{
    WindowCover &cover = WindowCover::Instance();
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("DownOrClose command received");
    if(cover.hasFeature(WindowCover::Feature::Lift)) {
        cover.Lift().TargetSet(0, WindowActuator::PositionUnits::Percentage100ths);
    }
    if(cover.hasFeature(WindowCover::Feature::Tilt)) {
        cover.Tilt().TargetSet(0, WindowActuator::PositionUnits::Percentage100ths);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}


/**
 * @brief Window Covering Cluster Stop Motion Command callback
 */
bool __attribute__((weak)) emberAfWindowCoveringClusterStopMotionCallback(chip::app::Command *)
{
    WindowCover &cover = WindowCover::Instance();
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("StopMotion command received");
    cover.Lift().Stop();
    cover.Tilt().Stop();
    emberAfSendImmediateDefaultResponse(status);
    return true;
}


/**
 * @brief Window Covering Cluster Go To Lift Value Command callback
 * @param liftValue
 */

bool __attribute__((weak)) emberAfWindowCoveringClusterGoToLiftValueCallback(chip::app::Command *cmmd, uint16_t value)
{
    WindowCover &cover = WindowCover::Instance();
    bool hasLift = cover.hasFeature(WindowCover::Feature::Lift);
    bool isPositionAware = cover.hasFeature(WindowCover::Feature::PositionAware);
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("GoToLiftValue Value command received");
    if(hasLift && isPositionAware) {
       cover.Lift().TargetSet(value, WindowActuator::PositionUnits::Value);
    }
    else {
        status = EMBER_ZCL_STATUS_ACTION_DENIED;
        emberAfWindowCoveringClusterPrint("Err Device is not PA=%u or LF=%u", isPositionAware, hasLift);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}


/**
 * @brief Window Covering Cluster Go To Lift Percentage Command callback
 * @param percentLiftValue
 */

bool __attribute__((weak)) emberAfWindowCoveringClusterGoToLiftPercentageCallback(chip::app::Command *cmmd, uint8_t percent)
{
    WindowCover &cover = WindowCover::Instance();
    bool hasLift = cover.hasFeature(WindowCover::Feature::Lift);
    bool isPositionAware = cover.hasFeature(WindowCover::Feature::PositionAware);
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("GoToLiftPercentage Percentage command received");
    if(hasLift && isPositionAware) {
       cover.Lift().TargetSet(percent, WindowActuator::PositionUnits::Percentage);
    }
    else {
        status = EMBER_ZCL_STATUS_ACTION_DENIED;
        emberAfWindowCoveringClusterPrint("Err Device is not PA=%u or LF=%u", isPositionAware, hasLift);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}

bool __attribute__((weak)) emberAfWindowCoveringClusterGoToLiftPercentageCallback(chip::app::Command *cmmd, uint8_t percent, uint16_t percent100ths)
{
    WindowCover &cover = WindowCover::Instance();
    bool hasLift = cover.hasFeature(WindowCover::Feature::Lift);
    bool isPositionAware = cover.hasFeature(WindowCover::Feature::PositionAware);
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("GoToLiftPercentage Percentage command received");
    if(hasLift && isPositionAware) {
        if(percent > 100) {
            cover.Lift().TargetSet(percent100ths, WindowActuator::PositionUnits::Percentage100ths);
        }
        else {
            cover.Lift().TargetSet(percent, WindowActuator::PositionUnits::Percentage);
        }
    }
    else {
        status = EMBER_ZCL_STATUS_ACTION_DENIED;
        emberAfWindowCoveringClusterPrint("Err Device is not PA=%u or LF=%u", isPositionAware, hasLift);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}


/**
 * @brief Window Covering Cluster Go To Tilt Value Command callback
 * @param tiltValue
 */

bool __attribute__((weak)) emberAfWindowCoveringClusterGoToTiltValueCallback(chip::app::Command *cmmd, uint16_t value)
{
    WindowCover &cover = WindowCover::Instance();
    bool hasTilt = cover.hasFeature(WindowCover::Feature::Tilt);
    bool isPositionAware = cover.hasFeature(WindowCover::Feature::PositionAware);
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("GoToTiltValue command received");
    if(hasTilt && isPositionAware) {
       cover.Tilt().TargetSet(value, WindowActuator::PositionUnits::Value);
    }
    else {
        status = EMBER_ZCL_STATUS_ACTION_DENIED;
        emberAfWindowCoveringClusterPrint("Err Device is not PA=%u or TL=%u", isPositionAware, hasTilt);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}


/**
 * @brief Window Covering Cluster Go To Tilt Percentage Command callback
 * @param percentTiltValue
 */

bool __attribute__((weak)) emberAfWindowCoveringClusterGoToTiltPercentageCallback(chip::app::Command *cmmd, uint8_t percent)
{
    WindowCover &cover = WindowCover::Instance();
    bool hasTilt = cover.hasFeature(WindowCover::Feature::Tilt);
    bool isPositionAware = cover.hasFeature(WindowCover::Feature::PositionAware);
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("GoToTiltPercentage command received");
    if(hasTilt && isPositionAware) {
       cover.Tilt().TargetSet(percent, WindowActuator::PositionUnits::Percentage);
    }
    else {
        status = EMBER_ZCL_STATUS_ACTION_DENIED;
        emberAfWindowCoveringClusterPrint("Err Device is not PA=%u or TL=%u", isPositionAware, hasTilt);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}


bool __attribute__((weak)) emberAfWindowCoveringClusterGoToTiltPercentageCallback(chip::app::Command *cmmd, uint8_t percent, uint16_t percent100ths)
{
    WindowCover &cover = WindowCover::Instance();
    bool hasTilt = cover.hasFeature(WindowCover::Feature::Tilt);
    bool isPositionAware = cover.hasFeature(WindowCover::Feature::PositionAware);
    EmberAfStatus status = EMBER_ZCL_STATUS_SUCCESS;

    emberAfWindowCoveringClusterPrint("GoToTiltPercentage command received");
    if(hasTilt && isPositionAware) {
        if(percent > 100) {
            cover.Tilt().TargetSet(percent100ths, WindowActuator::PositionUnits::Percentage100ths);
        }
        else {
            cover.Tilt().TargetSet(percent, WindowActuator::PositionUnits::Percentage);
        }
    }
    else {
        status = EMBER_ZCL_STATUS_ACTION_DENIED;
        emberAfWindowCoveringClusterPrint("Err Device is not PA=%u or TL=%u", isPositionAware, hasTilt);
    }
    emberAfSendImmediateDefaultResponse(status);
    return true;
}
