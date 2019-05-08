objcopy --only-keep-debug out out.debug
gdbserver :1234 out
