/* paths of input devices */
#define TOUCHSCREEN_POWER_PATH "/sys/class/input/input3/enabled"
#define TOUCHKEY_POWER_PATH "/sys/class/input/input8/enabled"

/* paths of interactive governor */
#define INTERACTIVE_PATH_L_ABOVE_HISPEED_DELAY "/sys/devices/system/cpu/cpu0/cpufreq/cafactive/above_hispeed_delay"
#define INTERACTIVE_PATH_L_BOOSTPULSE "/sys/devices/system/cpu/cpu0/cpufreq/cafactive/boostpulse"
#define INTERACTIVE_PATH_L_GO_HISPEED_LOAD "/sys/devices/system/cpu/cpu0/cpufreq/cafactive/go_hispeed_load"
#define INTERACTIVE_PATH_L_HISPEED_FREQ "/sys/devices/system/cpu/cpu0/cpufreq/cafactive/hispeed_freq"
#define INTERACTIVE_PATH_L_IO_IS_BUSY "/sys/devices/system/cpu/cpu0/cpufreq/cafactive/io_is_busy"
#define INTERACTIVE_PATH_L_TARGET_LOADS "/sys/devices/system/cpu/cpu0/cpufreq/cafactive/target_loads"
#define INTERACTIVE_PATH_B_ABOVE_HISPEED_DELAY "/sys/devices/system/cpu/cpu4/cpufreq/cafactive/above_hispeed_delay"
#define INTERACTIVE_PATH_B_BOOSTPULSE "/sys/devices/system/cpu/cpu4/cpufreq/cafactive/boostpulse"
#define INTERACTIVE_PATH_B_GO_HISPEED_LOAD "/sys/devices/system/cpu/cpu4/cpufreq/cafactive/go_hispeed_load"
#define INTERACTIVE_PATH_B_HISPEED_FREQ "/sys/devices/system/cpu/cpu4/cpufreq/cafactive/hispeed_freq"
#define INTERACTIVE_PATH_B_IO_IS_BUSY "/sys/devices/system/cpu/cpu4/cpufreq/cafactive/io_is_busy"
#define INTERACTIVE_PATH_B_TARGET_LOADS "/sys/devices/system/cpu/cpu4/cpufreq/cafactive/target_loads"

/* interactive normal profile */
#define INTERACTIVE_NORMAL_L_ABOVE_HISPEED_DELAY "39000 1200000:19000"
#define INTERACTIVE_NORMAL_L_GO_HISPEED_LOAD "89"
#define INTERACTIVE_NORMAL_L_HISPEED_FREQ "900000"
#define INTERACTIVE_NORMAL_L_TARGET_LOADS "75 1200000:85"
#define INTERACTIVE_NORMAL_B_ABOVE_HISPEED_DELAY "39000 1200000:19000"
#define INTERACTIVE_NORMAL_B_GO_HISPEED_LOAD "89"
#define INTERACTIVE_NORMAL_B_HISPEED_FREQ "900000"
#define INTERACTIVE_NORMAL_B_TARGET_LOADS "75 1200000:85"

/* interactive power-save profile */
#define INTERACTIVE_LOW_L_ABOVE_HISPEED_DELAY "39000 1200000:19000"
#define INTERACTIVE_LOW_L_GO_HISPEED_LOAD "89"
#define INTERACTIVE_LOW_L_HISPEED_FREQ "950000"
#define INTERACTIVE_LOW_L_TARGET_LOADS "75 1200000:85"
#define INTERACTIVE_LOW_B_ABOVE_HISPEED_DELAY "79000 1248000:99000 1664000:19000"
#define INTERACTIVE_LOW_B_GO_HISPEED_LOAD "99"
#define INTERACTIVE_LOW_B_HISPEED_FREQ "1040000"
#define INTERACTIVE_LOW_B_TARGET_LOADS "87 1664000:90"

/* interactive high-performance profile */
#define INTERACTIVE_HIGH_L_ABOVE_HISPEED_DELAY "19000"
#define INTERACTIVE_HIGH_L_GO_HISPEED_LOAD "75"
#define INTERACTIVE_HIGH_L_HISPEED_FREQ "1274000"
#define INTERACTIVE_HIGH_L_TARGET_LOADS "60"
#define INTERACTIVE_HIGH_B_ABOVE_HISPEED_DELAY "19000"
#define INTERACTIVE_HIGH_B_GO_HISPEED_LOAD "75"
#define INTERACTIVE_HIGH_B_HISPEED_FREQ "1456000"
#define INTERACTIVE_HIGH_B_TARGET_LOADS "70 1456000:80"
