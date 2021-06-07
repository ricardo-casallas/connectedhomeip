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

#ifdef __cplusplus
extern "C" {
#endif

/*
NOTE: These declarations are DUPLICATED on EVERY example!
*/
void efr32LogInit(void);
void efr32Log(const char * aFormat, ...);
void appError(int err);

#define EFR32_LOG(...) efr32Log(__VA_ARGS__);

#ifdef __cplusplus
}
#endif
