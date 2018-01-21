/*
   Copyright (c) 2016, The CyanogenMod Project. All rights reserved.

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
 */

#include <stdlib.h>
#include <string.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "vendor_init.h"

using android::base::GetProperty;
using android::base::ReadFileToString;
using android::base::Trim;

void property_override(char const prop[], char const value[])
{	
	prop_info *pi;

	pi = (prop_info*) __system_property_find(prop);
	if (pi)
		__system_property_update(pi, value, strlen(value));
	else
		__system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_sim_info()
{
	const char *simslot_count_path = "/proc/simslot_count";
	std::string simslot_count;
	
	if (ReadFileToString(simslot_count_path, &simslot_count)) {
		simslot_count = Trim(simslot_count); // strip newline
		property_override("ro.multisim.simslotcount", simslot_count.c_str());
		if (simslot_count.compare("2") == 0) {
			property_override("rild.libpath2", "/system/lib/libsec-ril-dsds.so");
			property_override("persist.radio.multisim.config", "dsds");
		}
	}
	else {
		LOG(ERROR) << "Could not open '" << simslot_count_path << "'\n";
	}
}

void vendor_load_properties()
{
	std::string platform;
	std::string bootloader = GetProperty("ro.bootloader", "");
	std::string device;

	platform = GetProperty("ro.board.platform", "");
	if (platform != ANDROID_TARGET)
		return;

	if (bootloader.find("A510F") != std::string::npos) {

		/* SM-A510F */
        property_override("ro.build.fingerprint", "samsung/a5xeltexx/a5xelte:5.1.1/LMY47X/A510FDXXS2APD1:user/release-keys");
        property_override("ro.build.description", "a5xeltexx-user 5.1.1 LMY47X A510FDXXS2APD1 release-keys");
        property_override("ro.product.model", "SM-A510F");
        property_override("ro.product.device", "a5xelte");

    } else if (bootloader.find("A510M") != std::string::npos) {

		/* SM-A510M */
        property_override("ro.build.fingerprint", "samsung/a5xeltexx/a5xelte:5.1.1/LMY47X/A510FXXS2APD1:user/release-keys");
        property_override("ro.build.description", "a5xeltexx-user 5.1.1 LMY47X A510FXXS2APD1 release-keys");
        property_override("ro.product.model", "SM-A510M");
        property_override("ro.product.device", "a5xelte");

    } else {

		/* SM-A510Y */
        property_override("ro.build.fingerprint", "samsung/a5xelteub/a5xelte:5.1.1/LMY47X/A510MUBS1APC1:user/release-keys");
        property_override("ro.build.description", "a5xelteub-user 5.1.1 LMY47X A510MUBS1APC1 release-keys");
        property_override("ro.product.model", "SM-A510Y");
        property_override("ro.product.device", "a5xelte");

	}

	set_sim_info();

	device = GetProperty("ro.product.device", "");
	LOG(ERROR) << "Found bootloader id '" << bootloader.c_str() << "' setting build properties for '" << device.c_str() << "' device\n";
}