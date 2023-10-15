load("//bazel/rules/cc:cc_firmware.bzl", "cc_firmware")

cc_library(
    name = "delay",
    srcs = ["delay.c"],
    hdrs = ["delay.h"],
    defines = ["F_CPU=16000000UL"],
)

cc_library(
    name = "uart",
    srcs = ["uart.c"],
    hdrs = ["uart.h"],
    defines = ["F_CPU=16000000UL"],
)

cc_library(
    name = "timer",
    srcs = ["timer.c"],
    hdrs = ["timer.h"],
)

cc_library(
    name = "log",
    srcs = ["log.c"],
    hdrs = ["log.h"],
    deps = [
        ":timer",
        ":uart",
    ]
)

cc_library(
    name = "enc28j60",
    srcs = ["enc28j60.c"],
    hdrs = ["enc28j60.h"],
    deps = [
        ":delay",
        ":spi"
    ],
)

cc_library(
    name = "spi",
    srcs = ["spi.c"],
    hdrs = ["spi.h"],
)

cc_binary(
    name = "ethernet",
    srcs = [
        "ethernet.c",
    ],
    deps = [
        ":delay",
        ":enc28j60",
        ":log",
        ":spi",
        ":timer",
        ":uart",
    ],
)

cc_firmware(
    name = "ethernet_firmware",
    src = ":ethernet",
)
