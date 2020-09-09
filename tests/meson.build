print_message_exe = executable(
    'print_message_test', 
    files('print_message.cpp'),
    dependencies: deps,
    link_args: warnings
)
test('print message test', print_message_exe)

ctrl_exe = executable(
    'ctrl_test', 
    files('ctrl.cpp'),
    dependencies: deps,
    link_args: warnings
)
test('ctrl test', ctrl_exe)
