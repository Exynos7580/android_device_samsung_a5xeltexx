
/*
 * Copyright (C) 2016 The CyanogenMod Project
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

enum device_variant {
    UNKNOWN = -1,
    A510F,
    A510M,
    A510Y,
    A510S,
    A510K,
    A510L
};

device_variant match(std::string bl)
{
    if (bl.find("A510F") != std::string::npos) {
        return A510F;
    } else if (bl.find("A510M") != std::string::npos) {
        return A510M;
    } else if (bl.find("A510Y") != std::string::npos) {
        return A510Y;
    } else if (bl.find("A510S") != std::string::npos) {
        return A510S;
    } else if (bl.find("A510K") != std::string::npos) {
        return A510K;
    } else if (bl.find("A510L") != std::string::npos) {
        return A510L;
    } else {
        return UNKNOWN;
    }
}
