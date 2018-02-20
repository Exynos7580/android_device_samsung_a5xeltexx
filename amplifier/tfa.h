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

#ifndef TFA_H
#define TFA_H

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#define TFA_LIBRARY_PATH "/system/lib/libtfa98xx.so"


/*
 * Amplifier audio modes for different usecases.
 */
typedef enum {
    Audio_Mode_None = -1,
    Audio_Mode_Music_Normal,
    Audio_Mode_Voice,
    Audio_Mode_Max
} tfa_mode_t;

/*
 * It doesn't really matter what this is, apparently we just need a continuous
 * chunk of memory...
 */
typedef struct {
    volatile int a1;
    volatile unsigned char a2[500];
} tfa_handle_t;

/*
 * Vendor functions that we dlsym.
 */
typedef int (*tfa_device_open_t)(tfa_handle_t*, int);
typedef int (*tfa_enable_t)(tfa_handle_t*, int);

/*
 * TFA Amplifier device abstraction.
 *
 * lib_handle:       The prebuilt vendor blob, loaded into memory
 * tfa_handle:       Misc data we need to pass to the vendor function calls
 * tfa_lock:         A mutex guarding amplifier enable/disable operations
 * tfa_device_open:  Vendor function for initializing the amplifier
 * tfa_enable:       Vendor function for enabling/disabling the amplifier
 * mode:             Audio mode for the current audio device
 */
typedef struct {
    void *lib_handle;
    tfa_handle_t* tfa_handle;
    pthread_mutex_t tfa_lock;
    tfa_device_open_t tfa_device_open;
    tfa_enable_t tfa_enable;
    tfa_mode_t mode;

    // for clock init
    atomic_bool initializing;
    bool clock_enabled;
    bool writing;
    pthread_t write_thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} tfa_device_t;

/*
 * Public API
 */
int tfa_power(tfa_device_t *tfa_dev, bool on);
tfa_device_t * tfa_device_open();
void tfa_device_close(tfa_device_t *tfa_dev);

#endif // TFA_H
