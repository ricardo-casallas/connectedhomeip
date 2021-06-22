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

#pragma once

#include "window-covering-common.h"
#include <app/common/gen/attribute-id.h>

struct WindowActuator
{
public:
    enum class PositionUnits {
        Value = 1,
        Percentage,
        Percentage100ths,
    };
    
    struct AttributeIds {
        uint16_t openLimit;
        uint16_t closedLimit;
        uint16_t positionValue;
        uint16_t positionPercent;
        uint16_t positionPercent100ths;
        uint16_t targetPercent100ths;
        uint16_t numberOfActuations;
    };

    // Actuator(uint16_t openLimit, uint16_t closedLimit, uint16_t delta);
    //  : mOpenLimit(openLimit), mClosedLimit(closedLimit), mStepDelta(delta) {};

    WindowActuator(AttributeIds ids) : mAttributeIds(ids) {}
    // ~WindowActuator() = default;

    void OpenLimitSet(uint16_t limit);
    uint16_t OpenLimitGet(void);

    void ClosedLimitSet(uint16_t limit);
    uint16_t ClosedLimitGet(void);

    void PositionSet(uint16_t position, PositionUnits units);
    void PositionSet(uint16_t position);
    uint16_t PositionGet(PositionUnits units);
    uint16_t PositionGet();

    void NumberOfActuationsIncrement();
    uint16_t NumberOfActuationsGet(void);

    void TargetSet(uint16_t position, PositionUnits units);
    void TargetSet(uint16_t position);
    uint16_t TargetGet(PositionUnits units);
    uint16_t TargetGet();

    void Stop(void) {};
    
protected:
    AttributeIds mAttributeIds;

    uint16_t ValueToPercent100ths(uint16_t position);
    uint16_t Percent100thsToValue(uint16_t percent100ths);
};

struct LiftActuator : public WindowActuator
{
public:
    LiftActuator() :
        WindowActuator(
            { ZCL_WC_INSTALLED_OPEN_LIMIT_LIFT_ATTRIBUTE_ID,
              ZCL_WC_INSTALLED_CLOSED_LIMIT_LIFT_ATTRIBUTE_ID,
              ZCL_WC_CURRENT_POSITION_LIFT_ATTRIBUTE_ID,
              ZCL_WC_CURRENT_POSITION_LIFT_PERCENTAGE_ATTRIBUTE_ID, 
              ZCL_WC_CURRENT_POSITION_LIFT_PERCENT100_THS_ATTRIBUTE_ID,
              ZCL_WC_TARGET_POSITION_LIFT_PERCENT100_THS_ATTRIBUTE_ID,
              ZCL_WC_NUMBER_OF_ACTUATIONS_LIFT_ATTRIBUTE_ID}) {}
};

struct TiltActuator : public WindowActuator
{
public:
    TiltActuator() :
        WindowActuator(
            { ZCL_WC_INSTALLED_OPEN_LIMIT_TILT_ATTRIBUTE_ID,
              ZCL_WC_INSTALLED_CLOSED_LIMIT_TILT_ATTRIBUTE_ID,
              ZCL_WC_CURRENT_POSITION_TILT_ATTRIBUTE_ID,
              ZCL_WC_CURRENT_POSITION_TILT_PERCENTAGE_ATTRIBUTE_ID, 
              ZCL_WC_CURRENT_POSITION_TILT_PERCENT100_THS_ATTRIBUTE_ID,
              ZCL_WC_TARGET_POSITION_TILT_PERCENT100_THS_ATTRIBUTE_ID,
              ZCL_WC_NUMBER_OF_ACTUATIONS_TILT_ATTRIBUTE_ID}) {}
};

class WindowCover
{
public:
    enum class Feature
    {
        Lift = 0x01,
        Tilt = 0x02,
        PositionAware = 0x04
    };

    static WindowCover &Instance();
    // static const char * TypeString(const CoverType type);

    bool hasFeature(Feature feat) {
        return (mFeatures & (uint8_t)feat) > 0;
    }

    LiftActuator &Lift() { return mLift;  }
    TiltActuator &Tilt() { return mTilt;  }

    // Attribute: Id  0 Type
    void TypeSet(EmberAfWcType type);
    EmberAfWcType TypeGet(void);

    // Attribute: Id  7 ConfigStatus
    void ConfigStatusSet(ConfigStatus_t status);
    ConfigStatus_t ConfigStatusGet(void);

    // Attribute: Id 10 OperationalStatus
    void OperationalStatusSet(OperationalStatus_t status);
    OperationalStatus_t OperationalStatusGet(void);

    // Attribute: Id 13 EndProductType
    void EndProductTypeSet(EmberAfWcEndProductType endProduct);
    EmberAfWcEndProductType EndProductTypeGet(void);

    // Attribute: Id 24 Mode
    void ModeSet(Mode_t mode);
    Mode_t ModeGet(void);

    // OPTIONAL Attributes -- Setter/Getter Internal Variables equivalent
    // Attribute: Id 27 SafetyStatus (Optional)
    void SafetyStatusSet(uint16_t status);
    uint16_t SafetyStatusGet(void);

    bool IsOpen();
    bool IsClosed();

private:
    static WindowCover sInstance;

    WindowCover();

    uint8_t mFeatures = ((uint8_t)Feature::Lift | (uint8_t)Feature::Tilt | (uint8_t)Feature::PositionAware);


    LiftActuator mLift;
    TiltActuator mTilt;
};
