/*
 * Copyright (C) 2017 The LineageOS Project
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

#include <telephony/ril.h>

#ifndef SAMSUNG_AUDIO_H
#define SAMSUNG_AUDIO_H

#define MIXER_CARD 0
#define SOUND_CARD 0

/* Playback */
#define SOUND_DEEP_BUFFER_DEVICE 6
#define SOUND_PLAYBACK_DEVICE 5
#define SOUND_PLAYBACK_SCO_DEVICE 3

/* Capture */
#define SOUND_CAPTURE_DEVICE 0
#define SOUND_CAPTURE_SCO_DEVICE 3

/* Voice calls */
#define SOUND_PLAYBACK_VOICE_DEVICE 2
#define SOUND_CAPTURE_VOICE_DEVICE 2

/* Wideband AMR callback */
#ifndef RIL_UNSOL_SNDMGR_WB_AMR_REPORT
#ifdef RIL_UNSOL_WB_AMR_STATE
#define RIL_UNSOL_SNDMGR_WB_AMR_REPORT RIL_UNSOL_WB_AMR_STATE
#else
#define RIL_UNSOL_SNDMGR_WB_AMR_REPORT 0
#endif
#endif

/* Unusupported
#define SOUND_CAPTURE_LOOPBACK_AEC_DEVICE 1
#define SOUND_CAPTURE_HOTWORD_DEVICE 0
*/


#endif // SAMSUNG_AUDIO_H
