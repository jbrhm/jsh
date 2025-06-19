# glob all of the utils
file(GLOB utils "*.cpp")

# create util executables
foreach(exec ${utils})
    get_filename_component(exec_name ${exec} NAME_WE)
    add_executable(${exec_name} ${exec})
endforeach()
