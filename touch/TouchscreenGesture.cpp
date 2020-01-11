/*
 * Copyright (C) 2020 The MoKee Open Source Project
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#define LOG_TAG "TouchscreenGestureService"

#include "TouchscreenGesture.h"
#include <android-base/logging.h>
#include <fstream>

#define GESTURE_CONTROL_PATH "/sys/class/meizu/tp/gesture_control"

#define SLIDE_LEFT_ENABLE   (1 << 0)
#define SLIDE_RIGHT_ENABLE  (1 << 1)
#define SLIDE_UP_ENABLE     (1 << 2)
#define SLIDE_DOWN_ENABLE   (1 << 3)
#define DOUBLE_TAP_ENABLE   (1 << 4)
#define ONECE_TAP_ENABLE    (1 << 5)
#define LONG_TAP_ENABLE     (1 << 6)
#define DRAW_E_ENABLE       (1 << 7)
#define DRAW_C_ENABLE       (1 << 8)
#define DRAW_W_ENABLE       (1 << 9)
#define DRAW_M_ENABLE       (1 << 10)
#define DRAW_O_ENABLE       (1 << 11)
#define DRAW_S_ENABLE       (1 << 12)
#define DRAW_V_ENABLE       (1 << 13)
#define DRAW_Z_ENABLE       (1 << 14)
#define FOD_ENABLE          (1 << 24)
#define ALL_GESTURE_ENABLE  (1 << 31)

namespace {
static std::string hex(uint32_t value) {
    char buf[4];
    snprintf(buf, sizeof(buf), "%08x", value);
    return buf;
}
}  // anonymous namespace

namespace vendor {
namespace mokee {
namespace touch {
namespace V1_0 {
namespace implementation {

const std::map<int32_t, TouchscreenGesture::GestureInfo> TouchscreenGesture::kGestureInfoMap = {
    {0,   {251, "one_finger_left_swipe", SLIDE_LEFT_ENABLE}},
    {1,   {252, "one_finger_right_swipe", SLIDE_RIGHT_ENABLE}},
    {2,   {254, "one_finger_up_swipe", SLIDE_UP_ENABLE}},
    {3,   {255, "one_finger_down_swipe", SLIDE_DOWN_ENABLE}},
    {4,   {253, "letter_e", DRAW_E_ENABLE}},
    {5,   {66,  "letter_c", DRAW_C_ENABLE}},
    {6,   {65,  "letter_w", DRAW_W_ENABLE}},
    {7,   {64,  "letter_m", DRAW_M_ENABLE}},
    {8,   {63,  "letter_o", DRAW_O_ENABLE}},
    {9,   {247, "letter_s", DRAW_S_ENABLE}},
    {10,  {250, "letter_v", DRAW_V_ENABLE}},
    {11,  {248, "letter_z", DRAW_Z_ENABLE}},
};

TouchscreenGesture::TouchscreenGesture() {
    mMainGestureControl = FOD_ENABLE | ALL_GESTURE_ENABLE;
    writeGestureControlValue();
}

Return<void> TouchscreenGesture::getSupportedGestures(getSupportedGestures_cb resultCb) {
    std::vector<Gesture> gestures;

    for (const auto& entry : kGestureInfoMap) {
        gestures.push_back({entry.first, entry.second.name, entry.second.keycode});
    }

    resultCb(gestures);
    return Void();
}

Return<bool> TouchscreenGesture::setGestureEnabled(
    const ::vendor::mokee::touch::V1_0::Gesture& gesture, bool enabled) {
    const auto entry = kGestureInfoMap.find(gesture.id);
    if (entry == kGestureInfoMap.end()) {
        return false;
    }

    const uint32_t value = entry->second.value;
    LOG(INFO) << "setGestureEnabled: " << hex(value) << enabled;

    if (enabled) {
      mMainGestureControl |= value;
    } else {
      mMainGestureControl &= value;
    }

    return writeGestureControlValue();
}

bool TouchscreenGesture::writeGestureControlValue() {
    std::ofstream file(GESTURE_CONTROL_PATH);
    file << mMainGestureControl;

    bool ret = !file.fail();
    LOG(INFO) << "writeGestureControlValue: " << hex(mMainGestureControl) << ", ret=" << ret;
    return ret;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace touch
}  // namespace mokee
}  // namespace vendor
