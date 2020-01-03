package msmnile

import (
    "android/soong/android"
)

func init() {
    android.RegisterModuleType("meizu_sm8150_fod_hal_binary", fodHalBinaryFactory)
}
