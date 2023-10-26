package(default_visibility = ["//visibility:public"])

cc_library(
    name = "clock",
    srcs = [
        "clock-arch.c",
        "clock-arch.h",
    ],
    hdrs = [
        "clock.h",
    ],
    includes = ["."],
    deps = ["//:timer"],
)

cc_library(
    name = "timer",
    srcs = [
        "timer.c",
    ],
    hdrs = [
        "timer.h",
    ],
    includes = ["."],
    deps = [":clock"],
)

cc_library(
    name = "uip",
    srcs = [
        "hello-world.c",
        "hello-world.h",
        "lc.h",
        "lc-switch.h",
        "psock.c",
        "psock.h",
        "pt.h",
        "uip.c",
        "uip.h",
        "uip-conf.h",
        "uip_arch.h",
        "uip_arp.c",
        "uip_arp.h",
        "uiplib.c",
        "uiplib.h",
        "uipopt.h",
    ],
    includes = ["."],
    deps = [
        ":timer",
    ],
)
