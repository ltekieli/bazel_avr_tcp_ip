package(default_visibility = ["//visibility:public"])

load("@bazel_skylib//rules:common_settings.bzl", "bool_flag")
load("//bazel/rules/cc:cc_firmware.bzl", "cc_firmware")

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
)

cc_library(
    name = "uart",
    srcs = ["uart.c"],
    hdrs = ["uart.h"],
    local_defines = [CPU_FREQ],
)

cc_library(
    name = "timer",
    srcs = ["timer.c"],
    hdrs = ["timer.h"],
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
    deps = [
        ":timer",
        ":uart",
    ],
)

cc_library(
    name = "enc28j60",
    srcs = ["enc28j60.c"],
    hdrs = ["enc28j60.h"],
    deps = [
        ":delay",
        ":spi",
    ],
)

cc_library(
    name = "spi",
    srcs = ["spi.c"],
    hdrs = ["spi.h"],
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
)

sh_binary(
    name = "program_bootloader",
    srcs = [
        "scripts/avrdude.sh",
    ],
    args = [
        "-f $(location @fastboot//:fastboot)",
    ],
    data = ["@fastboot"],
    tags = [
        "manual",
    ],
)
