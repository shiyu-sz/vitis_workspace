# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/sy/vitis_workspace/uart_intr_loop_wrapper/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/sy/vitis_workspace/uart_intr_loop_wrapper/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {uart_intr_loop_wrapper}\
-hw {/home/sy/vivado_project/uart_intr_loop/design_1_wrapper.xsa}\
-fsbl-target {psu_cortexa53_0} -out {/home/sy/vitis_workspace}

platform write
domain create -name {standalone_ps7_cortexa9_0} -display-name {standalone_ps7_cortexa9_0} -os {standalone} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {empty_application}
platform generate -domains 
platform active {uart_intr_loop_wrapper}
domain active {zynq_fsbl}
domain active {standalone_ps7_cortexa9_0}
platform generate -quick
platform generate
bsp reload
domain active {zynq_fsbl}
bsp reload
bsp reload
bsp reload
domain active {zynq_fsbl}
bsp reload
domain active {standalone_ps7_cortexa9_0}
bsp reload
platform active {uart_intr_loop_wrapper}
bsp reload
bsp config stdin "ps7_uart_1"
bsp config stdout "ps7_uart_1"
bsp write
bsp reload
catch {bsp regenerate}
platform generate -domains standalone_ps7_cortexa9_0 
