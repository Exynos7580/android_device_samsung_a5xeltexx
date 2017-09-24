/*
 * Copyright (C) 2013 The Android Open Source Project
 * Copyright (C) 2017 Jesse Chan <cjx123@outlook.com>
 * Copyright (C) 2017 Lukas Berger <mail@lukasberger.at>
 * Copyright (C) 2017 Victor Lourme <contact@zeroside.co>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <hardware/hardware.h>
#include <hardware/power.h>

using namespace std;

#ifndef EXYNOS5_POWER_HAL_H_INCLUDED
#define EXYNOS5_POWER_HAL_H_INCLUDED

/*
 * Macros
 */
#define PROFILE_SCREEN_OFF          -1
#define PROFILE_POWER_SAVE          0
#define PROFILE_BALANCED            1
#define PROFILE_HIGH_PERFORMANCE    2
#define PROFILE_BIAS_POWER_SAVE     3
#define PROFILE_BIAS_PERFORMANCE    4
#define PROFILE_MAX_USABLE          5

#define INPUT_STATE_DISABLE    0
#define INPUT_STATE_ENABLE     1

#define POWER_CONFIG_ALWAYS_ON_FP    "/data/power/always_on_fp"
#define POWER_CONFIG_DT2W            "/data/power/dt2w"
#define POWER_CONFIG_PROFILES        "/data/power/profiles"
#define POWER_CONFIG_BOOST           "/data/power/boost"

#define POWER_DT2W_ENABLED            "/sys/android_touch/doubletap2wake"
#define POWER_FINGERPRINT_ENABLED     "/sys/class/fingerprint/fingerprint/enabled"
#define POWER_TOUCHKEYS_ENABLED       "/sys/class/sec/sec_touchkey/input/enabled"
#define POWER_TOUCHSCREEN_ENABLED     "/sys/class/input/input3/enabled"
#define POWER_TOUCHKEYS_BRIGTHNESS    "/sys/class/sec/sec_touchkey/brightness"

/***********************************
 * Initializing
 */
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device);
static void power_init(struct power_module __unused * module);

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data);

/***********************************
 * Profiles
 */
static void power_set_profile(int profile);

/***********************************
 * Boost
 */
static void power_boostpulse(int duration);

/***********************************
 * Inputs
 */
static void power_input_device_state(int state);
static void power_set_interactive(struct power_module __unused * module, int on);

/***********************************
 * Features
 */
static int power_get_feature(struct power_module *module __unused, feature_t feature);
static void power_set_feature(struct power_module *module, feature_t feature, int state);

/***********************************
 * Utilities
 */
// C++ I/O
static bool pfwrite(string path, string str);
static bool pfwrite(string path, bool flag);
static bool pfwrite(string path, int value);
static bool pfwrite(string path, unsigned int value);
static bool pfwritegov(int core, string file, string str);
static bool pfwritegov(int core, string file, bool flag);
static bool pfwritegov(int core, string file, int value);
static bool pfwritegov(int core, string file, unsigned int value);
static bool pfread(string path, int *v);
static bool pfread(string path, string &str);

// legacy I/O
static bool pfwrite_legacy(string path, string str);
static bool pfwrite_legacy(string path, int value);
static bool pfwrite_legacy(string path, bool flag);

// I/O-helpers
static bool is_dir(string path);
static bool is_file(string path);

#endif // EXYNOS5_POWER_HAL_H_INCLUDED
