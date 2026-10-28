# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/sy/vitis_workspace/ov2640_to_hdmi_fpga_system/_ide/scripts/debugger_ov2640_to_hdmi_fpga-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/sy/vitis_workspace/ov2640_to_hdmi_fpga_system/_ide/scripts/debugger_ov2640_to_hdmi_fpga-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2 B176A1F9ABCD" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2-B176A1F9ABCD-23727093-0"}
fpga -file /home/sy/vitis_workspace/ov2640_to_hdmi_fpga/_ide/bitstream/ov2640_to_hdmi.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/sy/vitis_workspace/ov2640_to_hdmi_fpga_wrapper/export/ov2640_to_hdmi_fpga_wrapper/hw/ov2640_to_hdmi.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/sy/vitis_workspace/ov2640_to_hdmi_fpga/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
configparams force-mem-access 0
