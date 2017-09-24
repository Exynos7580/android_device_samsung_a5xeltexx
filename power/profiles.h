/*
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

using namespace std;

#ifndef EXYNOS5_POWER_HAL_PROFILES_INCLUDED
#define EXYNOS5_POWER_HAL_PROFILES_INCLUDED

struct power_profile_cpucluster {

	unsigned int freq_min;
	unsigned int freq_max;

	struct {
		bool cpu0;
		bool cpu1;
		bool cpu2;
		bool cpu3;
	} cores;

};

struct power_profile {

	struct {

		struct power_profile_cpucluster cl0;

		struct power_profile_cpucluster cl1;

	} cpu;

	struct {

		unsigned int min_lock;
		unsigned int max_lock;

	} gpu;

	struct {

		bool booster;
		string booster_table;

	} input;

	int ipa_control_temp;
	bool power_efficient_workqueue;

} power_profiles[PROFILE_MAX_USABLE + 1] = {

	/***********
	 * PROFILE_SCREEN_OFF
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 400000,
				.freq_min = 400000,
				.cores = {
					.cpu0 = true,
					.cpu1 = true,
					.cpu2 = true,
					.cpu3 = true,
				},
			},
			.cl1 = {
				.freq_max = 400000,
				.freq_min = 400000,
				.cores = {
					.cpu0 = false,
					.cpu1 = false,
					.cpu2 = false,
					.cpu3 = false,
				},
			},
		},
		.gpu = {
			.min_lock = 100,
			.max_lock = 100,
		},
		.input = {
			.booster = false,
			.booster_table = "0 0 0 0 0 0"
		},
		.ipa_control_temp = 35,
		.power_efficient_workqueue = true,
	},

	/***********
	 * PROFILE_POWER_SAVE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1100000,
				.freq_min = 200000,
				.cores = {
					.cpu0 = true,
					.cpu1 = true,
					.cpu2 = true,
					.cpu3 = true,
				},
			},
			.cl1 = {
				.freq_max = 1300000,
				.freq_min = 200000,
				.cores = {
                    .cpu0 = false,
                    .cpu1 = false,
                    .cpu2 = false,
                    .cpu3 = false,
                },
			},
		},
		.gpu = {
			.min_lock = 100,
			.max_lock = 440,
		},
		.input = {
			.booster = true,
			.booster_table = "300 0 400000 0 0 0"
		},
		.ipa_control_temp = 45,
		.power_efficient_workqueue = true,
	},

	/***********
	 * PROFILE_BALANCED
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1400000,
				.freq_min = 400000,
				.cores = {
                    .cpu0 = true,
                    .cpu1 = true,
                    .cpu2 = true,
                    .cpu3 = true,
                },
			},
			.cl1 = {
				.freq_max = 1500000,
				.freq_min = 400000,
				.cores = {
                    .cpu0 = true,
                    .cpu1 = true,
                    .cpu2 = true,
                    .cpu3 = true,
                },
			},
		},
		.gpu = {
			.min_lock = 100,
			.max_lock = 534,
		},
		.input = {
			.booster = true,
			.booster_table = "300 0 800000 0 0 0"
		},
		.ipa_control_temp = 65,
		.power_efficient_workqueue = true,
	},

	/***********
	 * PROFILE_HIGH_PERFORMANCE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1500000,
				.freq_min = 800000,
				.cores = {
                    .cpu0 = true,
                    .cpu1 = true,
                    .cpu2 = true,
                    .cpu3 = true,
                },
			},
			.cl1 = {
				.freq_max = 1500000,
				.freq_min = 800000,
				.cores = {
                    .cpu0 = true,
                    .cpu1 = true,
                    .cpu2 = true,
                    .cpu3 = true,
                },
			},
		},
		.gpu = {
			.min_lock = 440,
			.max_lock = 734,
		},
		.input = {
			.booster = true,
			.booster_table = "300 0 1200000 0 0 0"
		},
		.ipa_control_temp = 75,
		.power_efficient_workqueue = false,
	},

	/***********
	 * PROFILE_BIAS_POWER_SAVE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1100000,
				.freq_min = 600000,
				.cores = {
                    .cpu0 = true,
                    .cpu1 = true,
                    .cpu2 = true,
                    .cpu3 = true,
                },
			},
			.cl1 = {
				.freq_max = 1000000,
				.freq_min = 400000,
				.cores = {
                    .cpu0 = false,
                    .cpu1 = false,
                    .cpu2 = false,
                    .cpu3 = false,
                },
			},
		},
		.gpu = {
			.min_lock = 100,
			.max_lock = 350,
		},
		.input = {
			.booster = true,
			.booster_table = "300 0 600000 0 0 0"
		},
		.ipa_control_temp = 55,
		.power_efficient_workqueue = true,
	},

	/***********
	 * PROFILE_BIAS_PERFORMANCE
	 */
	{
		.cpu = {
			.cl0 = {
				.freq_max = 1500000,
				.freq_min = 1000000,
				.cores = {
                    .cpu0 = true,
                    .cpu1 = true,
                    .cpu2 = true,
                    .cpu3 = true,
                },
			},
			.cl1 = {
				.freq_max = 1500000,
				.freq_min = 600000,
				.cores = {
                    .cpu0 = true,
                    .cpu1 = true,
                    .cpu2 = true,
                    .cpu3 = true,
                },
			},
		},
		.gpu = {
			.min_lock = 266,
			.max_lock = 800,
		},
		.input = {
			.booster = true,
			.booster_table = "300 0 1000000 0 0 0"
		},
		.ipa_control_temp = 70,
		.power_efficient_workqueue = false,
	},

};

#endif // EXYNOS5_POWER_HAL_PROFILES_INCLUDED
