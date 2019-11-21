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

#define LOG_TAG "VibratorService"

#include <android-base/logging.h>

#include "Vibrator.h"

#define TIMED_OUTPUT_PATH "/sys/class/timed_output/vibrator/enable"
#define PREBAKED_EFFECT_PATH "/sys/class/meizu/motor/on_off"

namespace android {
namespace hardware {
namespace vibrator {
namespace V1_2 {
namespace implementation {

/*
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;

    file >> result;
    return file.fail() ? def : result;
}

Vibrator::Vibrator() {
}

// Methods from ::android::hardware::vibrator::V1_0::IVibrator follow.

Return<Status> Vibrator::on(uint32_t timeoutMs) {
    set(TIMED_OUTPUT_PATH, timeoutMs);
    return Status::OK;
}

Return<Status> Vibrator::off() {
    set(TIMED_OUTPUT_PATH, 0);
    return Status::OK;
}

Return<bool> Vibrator::supportsAmplitudeControl() {
    return false;
}

Return<Status> Vibrator::setAmplitude(uint8_t) {
    return Status::UNSUPPORTED_OPERATION;
}

Return<void> Vibrator::perform(V1_0::Effect effect, EffectStrength strength, perform_cb _hidl_cb) {
    return perform<decltype(effect)>(effect, strength, _hidl_cb);
}

// Methods from ::android::hardware::vibrator::V1_1::IVibrator follow.

Return<void> Vibrator::perform_1_1(V1_1::Effect_1_1 effect, EffectStrength strength,
                                   perform_cb _hidl_cb) {
    return perform<decltype(effect)>(effect, strength, _hidl_cb);
}

// Methods from ::android::hardware::vibrator::V1_2::IVibrator follow.

Return<void> Vibrator::perform_1_2(V1_2::Effect effect, EffectStrength strength,
                                   perform_cb _hidl_cb) {
    return perform<decltype(effect)>(effect, strength, _hidl_cb);
}

// Private methods follow.

Return<void> Vibrator::perform(Effect effect, EffectStrength, perform_cb _hidl_cb) {
    int id;
    uint32_t ms = 200;
    Status status = Status::OK;

    id = effectToVendorId(effect);
    if (id < 0) {
        LOG(ERROR) << "Perform: Effect not supported: " << effectToName(effect);
        _hidl_cb(status, 0);
        return Void();
    }

    set(PREBAKED_EFFECT_PATH, id);

    LOG(INFO) << "Perform: Effect " << effectToName(effect)
              << " => " << id;

    _hidl_cb(status, ms);

    return Void();
}

template <typename T>
Return<void> Vibrator::perform(T effect, EffectStrength strength, perform_cb _hidl_cb) {
    auto validRange = hidl_enum_range<T>();
    if (effect < *validRange.begin() || effect > *std::prev(validRange.end())) {
        _hidl_cb(Status::UNSUPPORTED_OPERATION, 0);
        return Void();
    }
    return perform(static_cast<Effect>(effect), strength, _hidl_cb);
}

const std::string Vibrator::effectToName(Effect effect) {
    return toString(effect);
}

int Vibrator::effectToVendorId(Effect effect) {
    switch (effect) {
        case Effect::CLICK:
            return 31008;
        case Effect::DOUBLE_CLICK:
            return 31003;
        case Effect::TICK:
            return 21000;
        case Effect::THUD:
            return 30900;
        case Effect::POP:
            return 22520;
        case Effect::HEAVY_CLICK:
            return 30900;
        default:
            return -1;
    }
}

}  // namespace implementation
}  // namespace V1_2
}  // namespace vibrator
}  // namespace hardware
}  // namespace android
