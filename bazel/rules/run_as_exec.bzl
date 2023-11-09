def _run_as_exec(ctx):
    local_executable = ctx.actions.declare_file(ctx.attr.name)
    ctx.actions.symlink(
        output = local_executable,
        target_file = ctx.executable.executable,
        is_executable = True,
    )

    return DefaultInfo(
        executable = local_executable,
        runfiles = ctx.runfiles(
            files = ctx.files.data,
        ),
    )

run_as_exec = rule(
    implementation = _run_as_exec,
    attrs = {
        "executable": attr.label(
            allow_files = True,
            cfg = "exec",
            executable = True,
            mandatory = True,
        ),
        "data": attr.label_list(
            allow_files = True,
            cfg = "target",
        ),
    },
    executable = True,
)
