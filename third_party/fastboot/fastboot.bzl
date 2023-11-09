load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

URLS_FASTBOOT = [
    "https://github.com/ltekieli/avr-fastboot/releases/download/2023.11.01/fastboot.tar.gz",
]

def fastboot():
    if "fastboot" not in native.existing_rules():
        http_archive(
            name = "fastboot",
            sha256 = "0b7f6b0f25a3da110af0511c91efebed4d167cca4d87f71fab7caf94ec0af468",
            urls = URLS_FASTBOOT,
            build_file = "//third_party/fastboot:fastboot.BUILD",
        )

URLS_FBOOT = [
    "https://github.com/ltekieli/avr-fastboot/archive/refs/tags/2023.11.01.tar.gz",
]

def fboot():
    if "fboot" not in native.existing_rules():
        http_archive(
            name = "fboot",
            sha256 = "bbd667e44c2453107227a6a0643090d9c45da0ae1c5ae5efbe543938df4f1165",
            urls = URLS_FBOOT,
            build_file = "//third_party/fastboot:fboot.BUILD",
            strip_prefix = "avr-fastboot-2023.11.01/FBoot-Linux/src",
        )
