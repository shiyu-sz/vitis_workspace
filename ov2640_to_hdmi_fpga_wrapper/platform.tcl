# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/sy/vitis_workspace/ov2640_to_hdmi_fpga_wrapper/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/sy/vitis_workspace/ov2640_to_hdmi_fpga_wrapper/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {ov2640_to_hdmi_fpga_wrapper}\
-hw {/home/sy/vivado_project/fpga/ov2640_to_hdmi/ov2640_to_hdmi.xsa}\
-fsbl-target {psu_cortexa53_0} -out {/home/sy/vitis_workspace}

platform write
domain create -name {standalone_ps7_cortexa9_0} -display-name {standalone_ps7_cortexa9_0} -os {standalone} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {empty_application}
platform generate -domains 
platform active {ov2640_to_hdmi_fpga_wrapper}
domain active {zynq_fsbl}
domain active {standalone_ps7_cortexa9_0}
platform generate -quick
platform generate
platform config -updatehw {/home/sy/vivado_project/fpga/ov2640_to_hdmi/ov2640_to_hdmi.xsa}
platform generate -domains 
