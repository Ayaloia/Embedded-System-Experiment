import os

Import("env")

# FRAMEWORK_DIR = env.PioPlatform().get_package_dir("framework-wd-riscv-sdk")
# patchflag_path = join(FRAMEWORK_DIR, ".rt-thread-patching-done")

# # patch file only if we didn't do it before
# if not isfile(join(FRAMEWORK_DIR, ".rt-thread-patching-done")):
#     original_file_dir = join(FRAMEWORK_DIR, "board", "nexys_a7_eh1")
#     patched_file = join("patch", "startup.patch")
#     # print("Patching %s with %s" % (original_file_dir, patched_file))
#     assert isdir(original_file_dir) and isfile(patched_file)

#     env.Execute(f"git apply --directory={original_file_dir} --unsafe-paths {patched_file}")
#     # env.Execute("touch " + patchflag_path)


#     def _touch(path):
#         with open(path, "w") as fp:
#             fp.write("")

#     env.Execute(lambda *args, **kwargs: _touch(patchflag_path))

rttthread_dir = os.path.join("lib", "rt-thread")
env.Append(
    CPPDEFINES=[
        "D_USE_RTTHREAD",
        "RTE_USING_FINSH",
    ],

    CPPPATH=[
        os.path.join(rttthread_dir, "bsp"),
        os.path.join(rttthread_dir, "components", "finsh"),
        os.path.join(rttthread_dir, "libcpu", "common"),
        os.path.join(rttthread_dir, "libcpu", "rv32m1"),
        os.path.join(rttthread_dir, "include", "libc"),
        os.path.join(rttthread_dir, "include"),
    ],

    LIBS=[
        env.BuildLibrary(
            os.path.join("$BUILD_DIR", "RT-THREAD"),
            os.path.join(rttthread_dir),
            src_filter=[
                "+<*>",
            ]
        ),
    ]
)
