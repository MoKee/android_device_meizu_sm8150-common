/*
 * Copyright (C) 2019 The Android Open Source Project
 * Copyright (C) 2019-2020 The MoKee Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "android.hardware.vibrator@1.2-service.meizu_sm8150"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>

#include "Vibrator.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::vibrator::V1_2::IVibrator;
using android::hardware::vibrator::V1_2::implementation::Vibrator;

int main() {
    android::sp<IVibrator> vibrator = new Vibrator();

    configureRpcThreadpool(1, true);

    android::status_t status =  vibrator->registerAsService();

    if (status != android::OK) {
        return status;
    }

    joinRpcThreadpool();

    return 1;
}
