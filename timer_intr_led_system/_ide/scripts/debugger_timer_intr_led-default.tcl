# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/sy/vitis_workspace/timer_intr_led_system/_ide/scripts/debugger_timer_intr_led-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/sy/vitis_workspace/timer_intr_led_system/_ide/scripts/debugger_timer_intr_led-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-SMT2 B176A1F9ABCD" && level==0 && jtag_device_ctx=="jsn-JTAG-SMT2-B176A1F9ABCD-23727093-0"}
fpga -file /home/sy/vitis_workspace/timer_intr_led/_ide/bitstream/design_1_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/sy/vitis_workspace/timer_intr_led_wrapper/export/timer_intr_led_wrapper/hw/design_1_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/sy/vitis_workspace/timer_intr_led/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/sy/vitis_workspace/timer_intr_led/Debug/timer_intr_led.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
