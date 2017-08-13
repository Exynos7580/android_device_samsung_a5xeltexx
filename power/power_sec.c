/*
 * Copyright (C) 2013 The Android Open Source Project
 * Copyright (C) 2017 Jesse Chan <cjx123@outlook.com>
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
 *
 */

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cutils/properties.h>

#define LOG_TAG "PowerHAL SEC"
#define LOG_NDEBUG 0
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

#include <power_sec.h>

/* index of profiles */
#define PROFILE_POWER_SAVE 0
#define PROFILE_NORMAL 1
#define PROFILE_HIGH_PERFORMANCE 2

/* index of states */
#define STATE_DISABLE 0
#define STATE_ENABLE 1

struct sec_power_module {
	struct power_module base;
	pthread_mutex_t lock;
	int boostpulse_fd_b;
	int boostpulse_fd_l;
	int boostpulse_warned_l;
	int boostpulse_warned_b;
};

#define container_of(addr, struct_name, field_name) \
	((struct_name *)((char *)(addr) - offsetof(struct_name, field_name)))

static int current_power_profile;

static int sysfs_write(const char *path, char *s)
{
	char buf[80];
	int len;
	int fd = open(path, O_WRONLY);

	if (fd < 0) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error opening %s: %s\n", path, buf);
		return fd;
	}

	len = write(fd, s, strlen(s));
	if (len < 0) {
		strerror_r(errno, buf, sizeof(buf));
		ALOGE("Error writing to %s: %s\n", path, buf);
	}

	close(fd);
	return len;
}

/*[Interactive CPUFreq Governor Functions]*/
static int is_interactive(void)
{
	int fd_l = open(INTERACTIVE_PATH_L_HISPEED_FREQ, O_WRONLY);
	int fd_b = open(INTERACTIVE_PATH_B_HISPEED_FREQ, O_WRONLY);

	if (fd_l < 0 || fd_b < 0)
		return 0;

	return 1;
}

static int interactive_set_profile(int profile)
{
	if (!(&is_interactive))
		return -EINVAL;

	switch (profile) {
	case PROFILE_POWER_SAVE:
		sysfs_write(INTERACTIVE_PATH_L_ABOVE_HISPEED_DELAY, INTERACTIVE_LOW_L_ABOVE_HISPEED_DELAY);
		sysfs_write(INTERACTIVE_PATH_L_GO_HISPEED_LOAD, INTERACTIVE_LOW_L_GO_HISPEED_LOAD);
		sysfs_write(INTERACTIVE_PATH_L_HISPEED_FREQ, INTERACTIVE_LOW_L_HISPEED_FREQ);
		sysfs_write(INTERACTIVE_PATH_L_TARGET_LOADS, INTERACTIVE_LOW_L_TARGET_LOADS);
		sysfs_write(INTERACTIVE_PATH_B_ABOVE_HISPEED_DELAY, INTERACTIVE_LOW_B_ABOVE_HISPEED_DELAY);
		sysfs_write(INTERACTIVE_PATH_B_GO_HISPEED_LOAD, INTERACTIVE_LOW_B_GO_HISPEED_LOAD);
		sysfs_write(INTERACTIVE_PATH_B_HISPEED_FREQ, INTERACTIVE_LOW_B_HISPEED_FREQ);
		sysfs_write(INTERACTIVE_PATH_B_TARGET_LOADS, INTERACTIVE_LOW_B_TARGET_LOADS);
                break;

	case PROFILE_NORMAL:
		sysfs_write(INTERACTIVE_PATH_L_ABOVE_HISPEED_DELAY, INTERACTIVE_NORMAL_L_ABOVE_HISPEED_DELAY);
		sysfs_write(INTERACTIVE_PATH_L_GO_HISPEED_LOAD, INTERACTIVE_NORMAL_L_GO_HISPEED_LOAD);
		sysfs_write(INTERACTIVE_PATH_L_HISPEED_FREQ, INTERACTIVE_NORMAL_L_HISPEED_FREQ);
		sysfs_write(INTERACTIVE_PATH_L_TARGET_LOADS, INTERACTIVE_NORMAL_L_TARGET_LOADS);
		sysfs_write(INTERACTIVE_PATH_B_ABOVE_HISPEED_DELAY, INTERACTIVE_NORMAL_B_ABOVE_HISPEED_DELAY);
		sysfs_write(INTERACTIVE_PATH_B_GO_HISPEED_LOAD, INTERACTIVE_NORMAL_B_GO_HISPEED_LOAD);
		sysfs_write(INTERACTIVE_PATH_B_HISPEED_FREQ, INTERACTIVE_NORMAL_B_HISPEED_FREQ);
		sysfs_write(INTERACTIVE_PATH_B_TARGET_LOADS, INTERACTIVE_NORMAL_B_TARGET_LOADS);
                break;

	case PROFILE_HIGH_PERFORMANCE:
		sysfs_write(INTERACTIVE_PATH_L_ABOVE_HISPEED_DELAY, INTERACTIVE_HIGH_L_ABOVE_HISPEED_DELAY);
		sysfs_write(INTERACTIVE_PATH_L_GO_HISPEED_LOAD, INTERACTIVE_HIGH_L_GO_HISPEED_LOAD);
		sysfs_write(INTERACTIVE_PATH_L_HISPEED_FREQ, INTERACTIVE_HIGH_L_HISPEED_FREQ);
		sysfs_write(INTERACTIVE_PATH_L_TARGET_LOADS, INTERACTIVE_HIGH_L_TARGET_LOADS);
		sysfs_write(INTERACTIVE_PATH_B_ABOVE_HISPEED_DELAY, INTERACTIVE_HIGH_B_ABOVE_HISPEED_DELAY);
		sysfs_write(INTERACTIVE_PATH_B_GO_HISPEED_LOAD, INTERACTIVE_HIGH_B_GO_HISPEED_LOAD);
		sysfs_write(INTERACTIVE_PATH_B_HISPEED_FREQ, INTERACTIVE_HIGH_B_HISPEED_FREQ);
		sysfs_write(INTERACTIVE_PATH_B_TARGET_LOADS, INTERACTIVE_HIGH_B_TARGET_LOADS);
                break;

	default:
		break;
	}

	return 0;
}

static void interactive_power_init(struct sec_power_module __unused * sec)
{
	/* initialize generic parameters first */
	sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_rate", "20000");
	sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/interactive/timer_slack", "20000");
	sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/interactive/min_sample_time", "40000");
	sysfs_write("/sys/devices/system/cpu/cpu0/cpufreq/interactive/boostpulse_duration", "40000");
	sysfs_write("/sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_rate", "20000");
	sysfs_write("/sys/devices/system/cpu/cpu4/cpufreq/interactive/timer_slack", "20000");
	sysfs_write("/sys/devices/system/cpu/cpu4/cpufreq/interactive/min_sample_time", "40000");
	sysfs_write("/sys/devices/system/cpu/cpu4/cpufreq/interactive/boostpulse_duration", "40000");

	/* use normal profile by default */
	interactive_set_profile(PROFILE_NORMAL);

	current_power_profile == PROFILE_NORMAL;
}

static int interactive_boostpulse(struct sec_power_module *sec)
{
	char buf[80];
	int len_l;
	int len_b;

	if (sec->boostpulse_fd_l < 0) {
		sec->boostpulse_fd_l = open(INTERACTIVE_PATH_L_BOOSTPULSE, O_WRONLY);
		if (sec->boostpulse_fd_l < 0) {
			if (!sec->boostpulse_warned_l) {
				strerror_r(errno, buf, sizeof(buf));
				ALOGE("Error opening %s: %s\n", INTERACTIVE_PATH_L_BOOSTPULSE, buf);
				sec->boostpulse_warned_l = 1;
			}
		}
	}

	if (sec->boostpulse_fd_b < 0) {
		sec->boostpulse_fd_b = open(INTERACTIVE_PATH_B_BOOSTPULSE, O_WRONLY);
		if (sec->boostpulse_fd_b < 0) {
			if (!sec->boostpulse_warned_b) {
				strerror_r(errno, buf, sizeof(buf));
				ALOGE("Error opening %s: %s\n", INTERACTIVE_PATH_B_BOOSTPULSE, buf);
				sec->boostpulse_warned_b = 1;
			}
		}
	}

	len_l = write(sec->boostpulse_fd_l, "1", 1);
	len_b = write(sec->boostpulse_fd_b, "1", 1);

	if (len_l < 0 || len_b < 0) {
		return -1;
	}

	return 0;
}

/*[Generic Functions]*/
static int set_input_device_state(int state)
{
	switch (state) {
		case STATE_DISABLE:
			sysfs_write(TOUCHSCREEN_POWER_PATH, "0");
			sysfs_write(TOUCHKEY_POWER_PATH, "0");
			break;

		case STATE_ENABLE:
			sysfs_write(TOUCHSCREEN_POWER_PATH, "1");
			sysfs_write(TOUCHKEY_POWER_PATH, "1");
			break;

		default:
			return -EINVAL;
	}

	return 0;
}

static void power_set_interactive(struct power_module __unused * module, int on)
{
	ALOGV("power_set_interactive: %d\n", on);

	set_input_device_state(on ? 1 : 0);

	/* Plug out big cores when screen is off */
	sysfs_write("/sys/module/lazyplug/parameters/nr_possible_cores", on ? "8" : "4");

	/*
	 * Switch to power-saving profile when screen is off.
	 */
	interactive_set_profile(on ? current_power_profile : PROFILE_POWER_SAVE);

	sysfs_write(INTERACTIVE_PATH_L_IO_IS_BUSY, on ? "1" : "0");
	sysfs_write(INTERACTIVE_PATH_B_IO_IS_BUSY, on ? "1" : "0");

	ALOGV("power_set_interactive: %d done\n", on);
}

static int set_power_profile(char *data)
{
	int profile = *((intptr_t *)data);
	int ret;

	switch (profile) {
		case PROFILE_POWER_SAVE:
			ret = interactive_set_profile(PROFILE_POWER_SAVE);
			current_power_profile = PROFILE_POWER_SAVE;
			break;

		case PROFILE_NORMAL:
			ret = interactive_set_profile(PROFILE_NORMAL);
			current_power_profile = PROFILE_NORMAL;
			break;

		case PROFILE_HIGH_PERFORMANCE:
			ret = interactive_set_profile(PROFILE_HIGH_PERFORMANCE);
			current_power_profile = PROFILE_HIGH_PERFORMANCE;
			break;

		default:
			return -EINVAL;
	}

	return ret;
}

static void sec_power_init(struct power_module __unused * module)
{
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	interactive_power_init(sec);
}

static void sec_power_hint(struct power_module *module, power_hint_t hint, void *data)
{
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	pthread_mutex_lock(&sec->lock);
	switch (hint) {
		case POWER_HINT_INTERACTION:
			interactive_boostpulse(sec);
			break;

		case POWER_HINT_VSYNC:
			break;

		case POWER_HINT_LOW_POWER:
			interactive_set_profile(data ? PROFILE_POWER_SAVE : current_power_profile);
			break;

		case POWER_HINT_DISABLE_TOUCH:
			set_input_device_state(data ? 0 : 1);
			break;

		case POWER_HINT_SET_PROFILE:
			set_power_profile(data);

		default:
			break;
	}
	pthread_mutex_unlock(&sec->lock);
}

static int get_feature(struct power_module *module __unused, feature_t feature)
{
	switch (feature) {
		case POWER_FEATURE_SUPPORTED_PROFILES:
		return 3;

		default:
		return -EINVAL;
	}
}

static void set_feature(struct power_module *module, feature_t feature, int state)
{
	struct sec_power_module *sec = container_of(module, struct sec_power_module, base);

	switch (feature) {
	default:
		ALOGW("Error setting the feature %d and state %d, it doesn't exist\n",
			  feature, state);
		break;
	}
}

static int power_open(const hw_module_t __unused * module, const char *name, hw_device_t **device)
{
	int retval = 0; /* 0 is ok; -1 is error */

	ALOGD("%s: enter; name=%s", __func__, name);

	if (strcmp(name, POWER_HARDWARE_MODULE_ID) == 0) {
		power_module_t *dev = (power_module_t *)calloc(1, sizeof(power_module_t));

		if (dev) {
			/* Common hw_device_t fields */
			dev->common.tag = HARDWARE_DEVICE_TAG;
			dev->common.module_api_version = POWER_MODULE_API_VERSION_0_5;
			dev->common.hal_api_version = HARDWARE_HAL_API_VERSION;

			dev->init = sec_power_init;
			dev->powerHint = sec_power_hint;
			dev->setInteractive = power_set_interactive;
			dev->setFeature = set_feature;

			*device = (hw_device_t *)dev;
		} else
			retval = -ENOMEM;
	} else {
		retval = -EINVAL;
	}

	ALOGD("%s: exit %d", __func__, retval);
	return retval;
}

static struct hw_module_methods_t power_module_methods = {
	.open = power_open,
};

struct sec_power_module HAL_MODULE_INFO_SYM = {
	.base = {
		.common = {
			.tag = HARDWARE_MODULE_TAG,
			.module_api_version = POWER_MODULE_API_VERSION_0_5,
			.hal_api_version = HARDWARE_HAL_API_VERSION,
			.id = POWER_HARDWARE_MODULE_ID,
			.name = "Power HAL SEC",
			.author = "Jesse Chan <cjx123@outlook.com>",
			.methods = &power_module_methods,
		},

		.init = sec_power_init,
		.setInteractive = power_set_interactive,
		.powerHint = sec_power_hint,
		.getFeature = get_feature,
		.setFeature = set_feature,
	},

	.lock = PTHREAD_MUTEX_INITIALIZER,
};
