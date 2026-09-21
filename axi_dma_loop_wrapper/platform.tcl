# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/sy/vitis_workspace/axi_dma_loop_wrapper/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/sy/vitis_workspace/axi_dma_loop_wrapper/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {axi_dma_loop_wrapper}\
-hw {/home/sy/vivado_project/axi_dma_loop/design_1_wrapper.xsa}\
-fsbl-target {psu_cortexa53_0} -out {/home/sy/vitis_workspace}

platform write
domain create -name {standalone_ps7_cortexa9_0} -display-name {standalone_ps7_cortexa9_0} -os {standalone} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {empty_application}
platform generate -domains 
platform active {axi_dma_loop_wrapper}
domain active {zynq_fsbl}
domain active {standalone_ps7_cortexa9_0}
platform generate -quick
bsp reload
bsp reload
platform generate
