load("//third_party/fastboot:fastboot.bzl", "fastboot", "fboot")
load("//third_party/skylib:skylib.bzl", "skylib")
load("//third_party/toolchains:toolchains.bzl", "toolchains")

def deps():
    fastboot()
    fboot()
    skylib()
    toolchains()
