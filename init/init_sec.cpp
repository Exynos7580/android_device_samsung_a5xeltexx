/*
   Copyright (c) 2015, The Dokdo Project. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   File Name : init_sec.c
   Create Date : 2015.11.03
   Author : Sunghun Ra
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "property_service.h"
#include "util.h"
#include "vendor_init.h"

#include "init_sec.h"

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

std::string bootloader;
std::string device;
char* devicename;

device_variant check_device_and_get_variant()
{
    FILE *file;
	char *simslot_count_path = "/proc/simslot_count";
	char simslot_count[2] = "\0";
	
	file = fopen(simslot_count_path, "r");
	
	if (file != NULL) {
		simslot_count[0] = fgetc(file);
		property_set("ro.multisim.simslotcount", simslot_count);
        
		if(strcmp(simslot_count, "2") == 0) {
			property_set("rild.libpath2", "/system/lib/libsec-ril-dsds.so");
			property_set("persist.radio.multisim.config", "dsds");
		}
        
		fclose(file);
	} else {
		ERROR("Could not open '%s'\n", simslot_count_path);
	}
    
    std::string platform = property_get("ro.board.platform");
    if (platform != ANDROID_TARGET) {
        return UNKNOWN;
    }

    bootloader = property_get("ro.bootloader");
    return match(bootloader);
}

void vendor_load_properties()
{
    device_variant variant = check_device_and_get_variant();

    switch (variant) {
        case A510F:
            /* a510f */
            property_set("ro.build.fingerprint", "samsung/a5xeltexx/a5xelte:7.0/NRD90M/A510FXXS4CQH4:user/release-keys");
            property_set("ro.build.description", "a5xeltexx-user 7.0 NRD90M A510FXXS4CQH4 release-keys");
            property_set("ro.product.model", "SM-A510F");
            property_set("ro.product.device", "a5xelte");
            break;
        case A510M:
            /* a510m */
            property_set("ro.build.fingerprint", "samsung/a5xelteub/a5xelte:7.0/NRD90M/A510MUBS3CQH1:user/release-keys");
            property_set("ro.build.description", "a5xelteub-user 7.0 NRD90M A510MUBS3CQH1 release-keys");
            property_set("ro.product.model", "SM-A510M");
            property_set("ro.product.device", "a5xelte");
            break;
        case A510Y:
            /* a510y */
            property_set("ro.build.fingerprint", "samsung/a5xeltedo/a5xelte:7.0/NRD90M/A510YDOS4CQF1:user/release-keys");
            property_set("ro.build.description", "a5xeltedo-user 7.0 NRD90M A510YDOS4CQF1 release-keys");
            property_set("ro.product.model", "SM-A510Y");
            property_set("ro.product.device", "a5xelte");
            break;
        case A510S:
            /* a510s */
            property_set("ro.build.fingerprint", "samsung/a5xelteskt/a5xelteskt:7.0/NRD90M/A510SKSU1CQF2:user/release-keys");
            property_set("ro.build.description", "a5xelteskt-user 7.0 NRD90M A510SKSU1CQF2 release-keys");
            property_set("ro.product.model", "SM-A510S");
            property_set("ro.product.device", "a5xelteskt");
            break;
        case A510K:
            /* a510k */
            property_set("ro.build.fingerprint", "samsung/a5xeltektt/a5xeltektt:7.0/NRD90M/A510KKKU1CQF2:user/release-keys");
            property_set("ro.build.description", "a5xeltektt-user 7.0 NRD90M A510KKKU1CQF2 release-keys");
            property_set("ro.product.model", "SM-A510K");
            property_set("ro.product.device", "a5xeltektt");
            break;
        case A510L:
            /* a510l */
            property_set("ro.build.fingerprint", "samsung/a5xeltelgt/a5xeltelgt:7.0/NRD90M/A510LKLU1CQF2:user/release-keys");
            property_set("ro.build.description", "a5xeltelgt-user 7.0 NRD90M A510LKLU1CQF2 release-keys");
            property_set("ro.product.model", "SM-A510L");
            property_set("ro.product.device", "a5xeltelgt");
            break;
        default:
            ERROR("Unknown bootloader id %s detected. bailing...\n", bootloader.c_str());
            return;
    }
    device = property_get("ro.product.device");
    INFO("Found bootloader id %s setting build properties for %s device\n", bootloader.c_str(), device.c_str());
}
