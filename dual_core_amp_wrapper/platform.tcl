# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/sy/vitis_workspace/dual_core_amp_wrapper/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/sy/vitis_workspace/dual_core_amp_wrapper/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {dual_core_amp_wrapper}\
-hw {/home/sy/vivado_project/dual_core_amp/design_1_wrapper.xsa}\
-fsbl-target {psu_cortexa53_0} -out {/home/sy/vitis_workspace}

platform write
domain create -name {standalone_ps7_cortexa9_0} -display-name {standalone_ps7_cortexa9_0} -os {standalone} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {empty_application}
platform generate -domains 
platform active {dual_core_amp_wrapper}
domain active {zynq_fsbl}
domain active {standalone_ps7_cortexa9_0}
platform generate -quick
domain create -name {standalone_ps7_cortexa9_1} -os {standalone} -proc {ps7_cortexa9_1} -arch {32-bit} -display-name {standalone_ps7_cortexa9_1} -desc {} -runtime {cpp}
platform generate -domains 
platform write
domain -report -json
platform generate
platform clean
platform generate
platform clean
platform generate
bsp reload
bsp config stdin "none"
bsp config stdout "none"
bsp write
bsp reload
catch {bsp regenerate}
platform generate -domains standalone_ps7_cortexa9_1 
bsp reload
bsp config stdin "ps7_uart_1"
bsp config stdout "ps7_uart_1"
bsp write
bsp reload
catch {bsp regenerate}
platform generate -domains standalone_ps7_cortexa9_1 
platform clean
platform generate
bsp reload
bsp reload
bsp reload
bsp reload
platform clean
platform generate
bsp reload
bsp reload
bsp config extra_compiler_flags "-mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -nostartfiles -g -Wall -Wextra -DUSE_AMP=1"
bsp write
bsp reload
catch {bsp regenerate}
platform clean
platform generate
platform clean
platform generate
platform config -updatehw {/home/sy/vivado_project/dual_core_amp/design_1_wrapper.xsa}
platform clean
platform generate
domain active {standalone_ps7_cortexa9_0}
bsp reload
bsp reload
domain active {standalone_ps7_cortexa9_1}
bsp reload
bsp reload
platform generate -domains 
