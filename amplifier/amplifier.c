/*
 * Copyright (C) 2017 Christopher N. Hesse <raymanfx@gmail.com>
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

#define LOG_TAG "audio_hw_amplifier"
//#define LOG_NDEBUG 1

#include <cutils/log.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <audio_hw.h>
#include <hardware/audio_amplifier.h>

#include "tfa.h"

typedef enum amp_state {
    AMP_STATE_DISABLED = 0,
    AMP_STATE_ENABLED,
    AMP_STATE_MAX = AMP_STATE_ENABLED
} amp_state_t;

typedef struct amp_device {
    amplifier_device_t amp_dev;
    tfa_device_t *tfa_dev;
    audio_mode_t current_mode;
    int refcount[Audio_Mode_Max];
    amp_state_t amp_state;
} amp_device_t;

static amp_device_t *amp_dev = NULL;

/*
 * Returns the internal TFA mode appropriate for the device.
 *
 * @param snd_device The current sound device.
 *
 * @return tfa_mode_t identifying the internal amplifier mode.
 */
static tfa_mode_t classify_snd_device(uint32_t snd_device) {
    tfa_mode_t mode = Audio_Mode_None;

    switch (snd_device) {
        case SND_DEVICE_OUT_SPEAKER:
        // our audio HAL splits this up
        //case SND_DEVICE_OUT_SPEAKER_AND_HEADPHONES:
            mode = Audio_Mode_Music_Normal;
            break;
        case SND_DEVICE_OUT_VOICE_SPEAKER:
        case SND_DEVICE_OUT_VOICE_SPEAKER_WB:
            mode = Audio_Mode_Voice;
            break;
        default:
            break;
    }

    return mode;
}

/*
 * Hook into amplifier HAL
 */
static int amp_enable_output_devices(amplifier_device_t *device,
        uint32_t devices, bool enable)
{
    amp_device_t *dev = (amp_device_t *) device;
    tfa_mode_t tfa_mode = classify_snd_device(devices);
    int rc = 0;

    ALOGV("%s: devices=0x%x, enable=%d, tfa_mode=%d", __func__, devices, enable, tfa_mode);

    if (tfa_mode == Audio_Mode_None) {
        return 0;
    }

    if (enable) {
        dev->refcount[tfa_mode]++;
    } else if (!enable && dev->refcount[tfa_mode] > 0) {
        dev->refcount[tfa_mode]--;
    }

    ALOGV("%s: enable=%d, dev->refcount=%d, amp_state=%d", __func__, enable,
          dev->refcount[tfa_mode], dev->amp_state);

    if (enable && dev->amp_state == AMP_STATE_DISABLED &&  dev->refcount[tfa_mode] == 1) {
        rc = tfa_power(dev->tfa_dev, true);
        if (rc == 0) {
            dev->amp_state = AMP_STATE_ENABLED;
        }
        ALOGV("%s: tfa_power(true) with rc=%d", __func__, rc);
    } else if (!enable && dev->amp_state == AMP_STATE_ENABLED && dev->refcount[tfa_mode] == 0) {
        rc = tfa_power(dev->tfa_dev, false);
        if (rc == 0) {
            dev->amp_state = AMP_STATE_DISABLED;
        }
        ALOGV("%s: tfa_power(false) with rc=%d", __func__, rc);
    }

    // TODO: Distinguish between tfa modes

    return rc;
}

static int amp_set_mode(amplifier_device_t *device, audio_mode_t mode)
{
    amp_device_t *dev = (amp_device_t *) device;

    dev->current_mode = mode;

    return 0;
}

static int amp_dev_close(hw_device_t *device)
{
    amp_device_t *dev = (amp_device_t *) device;

    tfa_device_close(dev->tfa_dev);

    free(dev);

    return 0;
}

static int amp_module_open(const hw_module_t *module, const char *name,
        hw_device_t **device)
{
    tfa_device_t *tfa_dev;

    if (strcmp(name, AMPLIFIER_HARDWARE_INTERFACE)) {
        ALOGE("%s: %s does not match amplifier hardware interface name",
                __func__, name);
        return -ENODEV;
    }

    if (amp_dev) {
        ALOGE("%s: Unable to open second instance of the amplifier", __func__);
        return -EBUSY;
    }

    tfa_dev = tfa_device_open();
    if (tfa_dev == NULL) {
        ALOGE("%s: Unable to open amplifier device", __func__);
        return -ENOENT;
    }

    amp_dev = calloc(1, sizeof(amp_device_t));
    if (!amp_dev) {
        ALOGE("%s: Unable to allocate memory for amplifier device", __func__);
        return -ENOMEM;
    }

    amp_dev->amp_dev.common.tag = HARDWARE_DEVICE_TAG;
    amp_dev->amp_dev.common.module = (hw_module_t *) module;
    amp_dev->amp_dev.common.version = HARDWARE_DEVICE_API_VERSION(1, 0);
    amp_dev->amp_dev.common.close = amp_dev_close;

    amp_dev->amp_dev.set_input_devices = NULL;
    amp_dev->amp_dev.set_output_devices = NULL;
    amp_dev->amp_dev.enable_input_devices = NULL;
    amp_dev->amp_dev.enable_output_devices = amp_enable_output_devices;
    amp_dev->amp_dev.set_mode = amp_set_mode;
    amp_dev->amp_dev.output_stream_start = NULL;
    amp_dev->amp_dev.input_stream_start = NULL;
    amp_dev->amp_dev.output_stream_standby = NULL;
    amp_dev->amp_dev.input_stream_standby = NULL;
    amp_dev->amp_dev.set_parameters = NULL;

    amp_dev->current_mode = AUDIO_MODE_NORMAL;
    memset(amp_dev->refcount, 0, Audio_Mode_Max);
    amp_dev->amp_state = AMP_STATE_DISABLED;

    amp_dev->tfa_dev = tfa_dev;

    *device = (hw_device_t *) amp_dev;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = amp_module_open,
};

amplifier_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AMPLIFIER_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AMPLIFIER_HARDWARE_MODULE_ID,
        .name = "Samsung TFA9897 amplifier HAL",
        .author = "Christopher N. Hesse",
        .methods = &hal_module_methods,
    },
};
