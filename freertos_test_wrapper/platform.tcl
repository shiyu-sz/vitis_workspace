# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/sy/vitis_workspace/freertos_test_wrapper/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/sy/vitis_workspace/freertos_test_wrapper/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {freertos_test_wrapper}\
-hw {/home/sy/vivado_project/freertos_test/design_1_wrapper.xsa}\
-fsbl-target {psu_cortexa53_0} -out {/home/sy/vitis_workspace}

platform write
domain create -name {freertos10_xilinx_ps7_cortexa9_0} -display-name {freertos10_xilinx_ps7_cortexa9_0} -os {freertos10_xilinx} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {freertos_hello_world}
platform generate -domains 
platform active {freertos_test_wrapper}
domain active {zynq_fsbl}
domain active {freertos10_xilinx_ps7_cortexa9_0}
platform generate -quick
platform generate
platform config -updatehw {/home/sy/vivado_project/freertos_test/design_1_wrapper.xsa}
platform generate -domains 
platform config -updatehw {/home/sy/vivado_project/freertos_test/design_1_wrapper.xsa}
platform clean
platform generate
bsp reload
bsp reload
platform config -updatehw {/home/sy/vivado_project/freertos_test/design_1_wrapper.xsa}
platform generate -domains 
