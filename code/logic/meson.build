dir = include_directories('.')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'c')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'cpp')

fossil_time_lib = library('fossil_time',
    files(
        'calendar.c',
        'timer.c',
        'sleep.c',
        'span.c',
        'date.c',
),
    install: true,
    include_directories: dir)

fossil_time_dep = declare_dependency(
    link_with: [fossil_time_lib],
    include_directories: dir)

meson.override_dependency('fossil-time', fossil_time_dep)