#
# Copyright (C) 2017 TeamNexus
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Include OSS Samsung Hardware-Projects (except ril)
include $(SAM_ROOT)/AdvancedDisplay/Android.mk
include $(SAM_ROOT)/audio/Android.mk
include $(SAM_ROOT)/consumerir/Android.mk
include $(SAM_ROOT)/dtbhtool/Android.mk
include $(SAM_ROOT)/fingerprint/Android.mk
include $(SAM_ROOT)/liblights/Android.mk
include $(SAM_ROOT)/modemloader/Android.mk
include $(SAM_ROOT)/power/Android.mk

# Include RIL-projects (except libril)
include $(SAM_ROOT)/ril/libsecril-client/Android.mk
include $(SAM_ROOT)/ril/libsecril-client-sap/Android.mk
