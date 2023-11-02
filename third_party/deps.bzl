load("//third_party/skylib:skylib.bzl", "skylib")
load("//third_party/toolchains:toolchains.bzl", "toolchains")

def deps():
    skylib()
    toolchains()
