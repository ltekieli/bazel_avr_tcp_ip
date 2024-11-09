load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load("@toolchain_avr//bazel/rules/cc:cc_firmware.bzl", "cc_firmware")
load("//bazel/rules:run_as_exec.bzl", "run_as_exec")

package(default_visibility = ["//visibility:public"])

##############################################################################
#
# Feature flags
#
##############################################################################
config_setting(
    name = "config_enable_logging",
    flag_values = {
        ":enable_logging": "true",
    },
)

bool_flag(
    name = "enable_logging",
    build_setting_default = False,
)

##############################################################################
#
# Constants
#
##############################################################################
CPU_FREQ = "F_CPU=16000000UL"

COMPATIBILITY = [
    "@toolchain_avr//bazel/platforms/cpu:avr",
    "@platforms//os:none",
]

HOST_COMPATIBILITY = [
    "@platforms//cpu:x86_64",
    "@platforms//os:linux",
]

##############################################################################
#
# Libraries
#
##############################################################################
cc_library(
    name = "delay",
    srcs = ["delay.c"],
    hdrs = ["delay.h"],
    defines = [CPU_FREQ],
    target_compatible_with = COMPATIBILITY,
)

cc_library(
    name = "uart",
    srcs = ["uart.c"],
    hdrs = ["uart.h"],
    local_defines = [CPU_FREQ],
    target_compatible_with = COMPATIBILITY,
)

cc_library(
    name = "timer",
    srcs = ["timer.c"],
    hdrs = ["timer.h"],
    target_compatible_with = COMPATIBILITY,
)

cc_library(
    name = "log",
    srcs = select({
        ":config_enable_logging": ["log.c"],
        "//conditions:default": [],
    }),
    hdrs = ["log.h"],
    defines = select({
        ":config_enable_logging": ["ENABLE_LOGGING=1"],
        "//conditions:default": [],
    }),
    target_compatible_with = COMPATIBILITY,
    deps = [
        ":timer",
        ":uart",
    ],
)

cc_library(
    name = "enc28j60",
    srcs = [
        "enc28j60.c",
        "enc28j60_defines.h",
    ],
    hdrs = ["enc28j60.h"],
    target_compatible_with = COMPATIBILITY,
    deps = [
        ":delay",
        ":spi",
    ],
)

cc_library(
    name = "spi",
    srcs = ["spi.c"],
    hdrs = ["spi.h"],
    target_compatible_with = COMPATIBILITY,
)

##############################################################################
#
# Firmware
#
##############################################################################
cc_binary(
    name = "ethernet",
    srcs = [
        "main.c",
    ],
    target_compatible_with = COMPATIBILITY,
    deps = [
        ":delay",
        ":enc28j60",
        ":log",
        ":spi",
        ":timer",
        ":uart",
        "//uip",
    ],
)

cc_firmware(
    name = "ethernet_firmware",
    src = ":ethernet",
    target_compatible_with = COMPATIBILITY,
)

##############################################################################
#
# Flashing commands
#
##############################################################################
sh_binary(
    name = "program_bootloader",
    srcs = [
        "scripts/avrdude.sh",
    ],
    args = [
        "-f $(location @fastboot//:fastboot)",
    ],
    data = ["@fastboot"],
)

run_as_exec(
    name = "program_application",
    args = [
        "-d /dev/ttyUSB0",
        "-b 230400",
        "-P Peda",
        "-p $(location :ethernet_firmware)",
    ],
    data = [
        ":ethernet_firmware",
    ],
    executable = "@fboot//:fboot",
)
