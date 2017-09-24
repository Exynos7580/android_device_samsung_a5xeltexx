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

#define LOG_TAG "power.exynos5"
#define LOG_NDEBUG 0

#include <atomic>
#include <cutils/properties.h>
#include <fcntl.h>
#include <fstream>
#include <future>
#include <hardware/hardware.h>
#include <hardware/power.h>
#include <iostream>
#include <linux/stat.h>
#include <math.h>
#include <pwd.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <utils/Log.h>

#include "power.h"
#include "profiles.h"

using namespace std;

struct sec_power_module {
	struct power_module base;
	pthread_mutex_t lock;
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int current_power_profile = PROFILE_BALANCED;
static int requested_power_profile = PROFILE_BALANCED;

static int input_state_touchkeys = 1;

/***********************************
 * Initializing
 */
static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device) {
	int retval = 0; // 0 is ok; -1 is error

	ALOGD("%s: enter; name=%s", __func__, name);

	if (strcmp(name, POWER_HARDWARE_MODULE_ID) == 0) {
		power_module_t *dev = (power_module_t *)calloc(1, sizeof(power_module_t));

		if (dev) {
			// Common hw_device_t fields
			dev->common.tag = HARDWARE_DEVICE_TAG;
			dev->common.module_api_version = POWER_MODULE_API_VERSION_0_5;
			dev->common.hal_api_version = HARDWARE_HAL_API_VERSION;

			dev->init = power_init;
			dev->powerHint = power_hint;
			dev->getFeature = power_get_feature;
			dev->setFeature = power_set_feature;
			dev->setInteractive = power_set_interactive;

			*device = (hw_device_t *)dev;
		} else {
			retval = -ENOMEM;
		}
	} else {
		retval = -EINVAL;
	}

	ALOGD("%s: exit %d", __func__, retval);
	return retval;
}

static void power_init(struct power_module __unused * module) {
	/*
	if (!is_file(POWER_CONFIG_ALWAYS_ON_FP))
		pfwrite(POWER_CONFIG_ALWAYS_ON_FP, false);

	*/

	if (!is_file(POWER_CONFIG_DT2W))
		pfwrite(POWER_CONFIG_DT2W, false);

	if (!is_file(POWER_CONFIG_BOOST))
		pfwrite(POWER_CONFIG_BOOST, true);

	if (!is_file(POWER_CONFIG_PROFILES))
		pfwrite(POWER_CONFIG_PROFILES, true);

	// set to normal power profile
	power_set_profile(PROFILE_BALANCED);

	// initialize all input-devices
	power_input_device_state(1);

	// set the default settings
	if (!is_dir("/data/power"))
		mkdir("/data/power", 0771);
}

/***********************************
 * Hinting
 */
static void power_hint(struct power_module *module, power_hint_t hint, void *data) {
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);
	int value = (data ? *((intptr_t *)data) : 0);

	pthread_mutex_lock(&sec->lock);

	switch (hint) {

		/***********************************
		 * Profiles
		 */
		case POWER_HINT_LOW_POWER:
			ALOGI("%s: hint(POWER_HINT_LOW_POWER, %d, %llu)", __func__, value, (unsigned long long)data);
			power_set_profile(value ? PROFILE_POWER_SAVE : requested_power_profile);
			break;

		case POWER_HINT_SET_PROFILE:
			ALOGI("%s: hint(POWER_HINT_SET_PROFILE, %d, %llu)", __func__, value, (unsigned long long)data);
			requested_power_profile = value;
			power_set_profile(value);
			break;

		case POWER_HINT_SUSTAINED_PERFORMANCE:
		case POWER_HINT_VR_MODE:
			if (hint == POWER_HINT_SUSTAINED_PERFORMANCE)
				ALOGI("%s: hint(POWER_HINT_SUSTAINED_PERFORMANCE, %d, %llu)", __func__, value, (unsigned long long)data);
			else if (hint == POWER_HINT_VR_MODE)
				ALOGI("%s: hint(POWER_HINT_VR_MODE, %d, %llu)", __func__, value, (unsigned long long)data);

			power_set_profile(value ? PROFILE_HIGH_PERFORMANCE : requested_power_profile);
			break;

		/***********************************
		 * Boosting
		 */
		case POWER_HINT_INTERACTION:
			ALOGI("%s: hint(POWER_HINT_INTERACTION, %d, %llu)", __func__, value, (unsigned long long)data);

			power_boostpulse(value ? value : 50000);
			power_boostpulse(value ? value : 50000);

			break;

	        case POWER_HINT_CPU_BOOST:
			ALOGI("%s: hint(POWER_HINT_INTERACTION, %d, %llu)", __func__, value, (unsigned long long)data);

			power_boostpulse(value);
			power_boostpulse(value);

			break;

		/***********************************
		 * Inputs
		 */
		case POWER_HINT_DISABLE_TOUCH:
			ALOGI("%s: hint(POWER_HINT_DISABLE_TOUCH, %d, %llu)", __func__, value, (unsigned long long)data);
			power_input_device_state(value ? 0 : 1);
			break;

		default: break;
	}

	pthread_mutex_unlock(&sec->lock);
}

/***********************************
 * Profiles
 */
static void power_set_profile(int profile) {
	int profiles = 1;

	pfread(POWER_CONFIG_PROFILES, &profiles);
	if (!profiles) {
		ALOGD("%s: profiles disabled (tried to apply profile %d)", __func__, profile);
		return;
	}

 	ALOGD("%s: apply profile %d", __func__, profile);

	// store it
	current_power_profile = profile;

	// apply settings
	struct power_profile data = power_profiles[current_power_profile + 1];

	/*********************
	 * CPU Cluster0
	 */

	// Cores
	pfwrite("/sys/devices/system/cpu/cpu0/online", data.cpu.cl0.cores.cpu0);
	pfwrite("/sys/devices/system/cpu/cpu1/online", data.cpu.cl0.cores.cpu1);
	pfwrite("/sys/devices/system/cpu/cpu2/online", data.cpu.cl0.cores.cpu2);
	pfwrite("/sys/devices/system/cpu/cpu3/online", data.cpu.cl0.cores.cpu3);

	// settings
	pfwritegov(0, "freq_min",     data.cpu.cl0.freq_min); /* Core, File, Value */
	pfwritegov(0, "freq_max",     data.cpu.cl0.freq_max);
	pfwritegov(0, "hispeed_freq", data.cpu.cl0.freq_max);

	/*********************
	 * CPU Cluster1
	 */
	// Cores
	pfwrite("/sys/devices/system/cpu/cpu4/online", data.cpu.cl1.cores.cpu0);
	pfwrite("/sys/devices/system/cpu/cpu5/online", data.cpu.cl1.cores.cpu1);
	pfwrite("/sys/devices/system/cpu/cpu6/online", data.cpu.cl1.cores.cpu2);
	pfwrite("/sys/devices/system/cpu/cpu7/online", data.cpu.cl1.cores.cpu3);

	// Settings
	pfwritegov(4, "freq_min",     data.cpu.cl1.freq_min); /* Core, File, Value */
	pfwritegov(4, "freq_max",     data.cpu.cl1.freq_max);
	pfwritegov(4, "hispeed_freq", data.cpu.cl1.freq_max);

	/*********************
	 * GPU
	 */
	pfwrite("/sys/devices/11400000.mali/dvfs_min_lock", data.gpu.min_lock);
	pfwrite("/sys/devices/11400000.mali/dvfs_max_lock", data.gpu.max_lock);

	/*********************
	 * Input
	 */
	pfwrite_legacy("/sys/class/input_booster/level", (data.input.booster ? 2 : 0));
	pfwrite_legacy("/sys/class/input_booster/head", data.input.booster_table);
	pfwrite_legacy("/sys/class/input_booster/tail", data.input.booster_table);

	/*********************
	 * Generic Settings
	 */
	pfwrite("/sys/power/enable_dm_hotplug", false);
	pfwrite("/sys/power/ipa/control_temp", data.ipa_control_temp);
	pfwrite("/sys/module/workqueue/parameters/power_efficient", data.power_efficient_workqueue);
}

/***********************************
 * Boost
 */
static void power_boostpulse(int duration) {
	int boost = 0;

	pfread(POWER_CONFIG_BOOST, &boost);
	if (!boost) {
		return;
	}

	ALOGD("%s: duration     = %d", __func__, duration);

	if (duration > 0) {
		pfwritegov(0, "boostpulse_duration", duration);
		pfwritegov(1, "boostpulse_duration", duration);
	}

	pfwritegov(0, "boostpulse", true);
	pfwritegov(1, "boostpulse", true);
}

/***********************************
 * Inputs
 */
static void power_input_device_state(int state) {
	int dt2w = 0, dt2w_sysfs = 0;
	//int always_on_fp = 0;

	pfread(POWER_CONFIG_DT2W, &dt2w);
	pfread(POWER_DT2W_ENABLED, &dt2w_sysfs);
	//pfread(POWER_CONFIG_ALWAYS_ON_FP, &always_on_fp);

#if LOG_NDEBUG
	ALOGD("%s: state         = %d", __func__, state);
	ALOGD("%s: dt2w          = %d", __func__, dt2w);
	ALOGD("%s: dt2w_sysfs    = %d", __func__, dt2w_sysfs);
	//ALOGD("%s: always_on_fp  = %d", __func__, always_on_fp);
#endif

	switch (state) {
		case INPUT_STATE_DISABLE:

			// save to current state to prevent enabling
			pfread(POWER_TOUCHKEYS_ENABLED, &input_state_touchkeys);

			pfwrite(POWER_TOUCHSCREEN_ENABLED, false);
			pfwrite(POWER_TOUCHKEYS_ENABLED, false);
			pfwrite(POWER_TOUCHKEYS_BRIGTHNESS, 0);

			/*
			if (always_on_fp) {
				pfwrite(POWER_FINGERPRINT_ENABLED, true);
			} else {
				pfwrite(POWER_FINGERPRINT_ENABLED, false);
			}*/

			break;

		case INPUT_STATE_ENABLE:

			pfwrite(POWER_TOUCHSCREEN_ENABLED, true);
			//pfwrite(POWER_FINGERPRINT_ENABLED, true);

			if (input_state_touchkeys) {
				pfwrite(POWER_TOUCHKEYS_ENABLED, true);
				pfwrite(POWER_TOUCHKEYS_BRIGTHNESS, 255);
			}

			break;
	}

	if (dt2w) {
		pfwrite_legacy(POWER_DT2W_ENABLED, true);
	} else {
		pfwrite_legacy(POWER_DT2W_ENABLED, false);
	}

	// give hw some milliseconds to properly boot up
	usleep(100 * 1000); // 100ms
}

static void power_set_interactive(struct power_module __unused * module, int on) {
	int screen_is_on = (on != 0);
#if LOG_NDEBUG
	ALOGD("%s: on = %d", __func__, on);
#endif

	if (!screen_is_on) {
		power_set_profile(PROFILE_SCREEN_OFF);
	} else {
		power_set_profile(requested_power_profile);
	}

	power_input_device_state(screen_is_on);
}

/***********************************
 * Features
 */
static int power_get_feature(struct power_module *module __unused, feature_t feature) {
	switch (feature) {
		case POWER_FEATURE_SUPPORTED_PROFILES:
			ALOGD("%s: request for POWER_FEATURE_SUPPORTED_PROFILES = %d", __func__, PROFILE_MAX_USABLE);
			return PROFILE_MAX_USABLE;
		case POWER_FEATURE_DOUBLE_TAP_TO_WAKE:
			ALOGD("%s: request for POWER_FEATURE_DOUBLE_TAP_TO_WAKE = 1", __func__);
			return 1;
		default:
			return -EINVAL;
	}
}

static void power_set_feature(struct power_module *module, feature_t feature, int state) {
	switch (feature) {
		case POWER_FEATURE_DOUBLE_TAP_TO_WAKE:
			ALOGD("%s: set POWER_FEATURE_DOUBLE_TAP_TO_WAKE to \"%d\"", __func__, state);
			if (state) {
				pfwrite(POWER_CONFIG_DT2W, true);
				pfwrite_legacy(POWER_DT2W_ENABLED, true);
			} else {
				pfwrite(POWER_CONFIG_DT2W, false);
				pfwrite_legacy(POWER_DT2W_ENABLED, false);
			}
			break;
		default:
			ALOGW("Error setting the feature %d and state %d, it doesn't exist\n",
				  feature, state);
		break;
	}
}

/***********************************
 * Utilities
 */
// C++ I/O
static bool pfwrite(string path, string str) {
	ofstream file;

	file.open(path);
	if (!file.is_open()) {
		ALOGE("%s: failed to open %s", __func__, path.c_str());
		return false;
	}

#if LOG_NDEBUG
	ALOGI("%s: store \"%s\" to %s", __func__, str.c_str(), path.c_str());
#endif
	file << str;
	file.close();

	return true;
}

static bool pfwrite(string path, bool flag) {
	return pfwrite(path, flag ? 1 : 0);
}

static bool pfwrite(string path, int value) {
	return pfwrite(path, to_string(value));
}

static bool pfwrite(string path, unsigned int value) {
	return pfwrite(path, to_string(value));
}

static bool pfwritegov(int core, string file, string str) {
	string cpugov;
	ostringstream path;
	ostringstream cpugov_path;

	path << "/sys/devices/system/cpu/cpu" << core << "/cpufreq";
	cpugov_path << path.str() << "/scaling_governor";

	pfread(cpugov_path.str(), cpugov);

	path << "/" << cpugov << "/" << file;

	if (!is_file(path.str())) {
		return false;
	}

	return pfwrite(path.str(), str);
}

static bool pfwritegov(int core, string file, bool flag) {
	return pfwritegov(core, file, flag ? 1 : 0);
}

static bool pfwritegov(int core, string file, int value) {
	return pfwritegov(core, file, to_string(value));
}

static bool pfwritegov(int core, string file, unsigned int value) {
	return pfwritegov(core, file, to_string(value));
}

static bool pfread(string path, int *v) {
	ifstream file(path);
	string line;

	if (!file.is_open()) {
		ALOGE("%s: failed to open %s", __func__, path.c_str());
		return false;
	}

	if (!getline(file, line)) {
		ALOGE("%s: failed to read from %s", __func__, path.c_str());
		return false;
	}

	file.close();
	*v = stoi(line);

	return true;
}

static bool pfread(string path, string &str) {
	ifstream file(path);

	if (!file.is_open()) {
		ALOGE("%s: failed to open %s", __func__, path.c_str());
		return false;
	}

	if (!getline(file, str)) {
		ALOGE("%s: failed to read from %s", __func__, path.c_str());
		return false;
	}

	file.close();
	return true;
}

// legacy I/O
static bool pfwrite_legacy(string path, string str) {
	FILE *file = fopen(path.c_str(), "w");
	bool ret = true;

	if (file == NULL) {
		ALOGE("%s: failed to open %s", __func__, path.c_str());
		return false;
	}

	fprintf(file, "%s\n", str.c_str());

	if (ferror(file)) {
		ALOGE("%s: failed to write to %s", __func__, path.c_str());
		ret = false;
	}

	fclose(file);
	return ret;
}

static bool pfwrite_legacy(string path, int value) {
	return pfwrite_legacy(path, to_string(value));
}

static bool pfwrite_legacy(string path, bool flag) {
	return pfwrite_legacy(path, flag ? 1 : 0);
}

// existence-helpers
static bool is_dir(string path) {
	struct stat fstat;
	const char *cpath = path.c_str();

	return !stat(cpath, &fstat) &&
		(fstat.st_mode & S_IFDIR) == S_IFDIR;
}

static bool is_file(string path) {
	struct stat fstat;
	const char *cpath = path.c_str();

	return !stat(cpath, &fstat) &&
		(fstat.st_mode & S_IFREG) == S_IFREG;
}

static struct hw_module_methods_t power_module_methods = {
	.open = power_open,
};

struct sec_power_module HAL_MODULE_INFO_SYM = {
	.base = {
		.common = {
			.tag = HARDWARE_MODULE_TAG,
#ifdef HAS_LAUNCH_HINT_SUPPORT
			.module_api_version = POWER_MODULE_API_VERSION_0_6,
#else
			.module_api_version = POWER_MODULE_API_VERSION_0_5,
#endif
			.hal_api_version = HARDWARE_HAL_API_VERSION,
			.id = POWER_HARDWARE_MODULE_ID,
			.name = "Power HAL for Exynos 7580 SoCs",
			.author = "Victor Lourme <contact@zeroside.co>",
			.methods = &power_module_methods,
		},

		.init = power_init,
		.powerHint = power_hint,
		.getFeature = power_get_feature,
		.setFeature = power_set_feature,
		.setInteractive = power_set_interactive,
	},

	.lock = PTHREAD_MUTEX_INITIALIZER
};
