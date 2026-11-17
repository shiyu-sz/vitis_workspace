# 2026-11-13T15:41:18.816329688
import vitis

client = vitis.create_client()
client.set_workspace(path="vitis_2025_2")

platform = client.create_platform_component(name = "ov2640_to_hdmi_platform",hw_design = "$COMPONENT_LOCATION/../../../vivado_project/fpga_2025_2/ov2640_to_hdmi/design_1_wrapper.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0",compiler = "gcc")

comp = client.create_app_component(name="ov2640_to_hdmi",platform = "$COMPONENT_LOCATION/../ov2640_to_hdmi_platform/export/ov2640_to_hdmi_platform/ov2640_to_hdmi_platform.xpfm",domain = "standalone_ps7_cortexa9_0")

platform = client.get_component(name="ov2640_to_hdmi_platform")
status = platform.build()

comp = client.get_component(name="ov2640_to_hdmi")
comp.build()

platform = client.create_platform_component(name = "hello_world_platform",hw_design = "$COMPONENT_LOCATION/../../../vivado_project/vitis_2025_2/hello_world/design_1_wrapper.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0",compiler = "gcc")

comp = client.create_app_component(name="hello_world",platform = "$COMPONENT_LOCATION/../hello_world_platform/export/hello_world_platform/hello_world_platform.xpfm",domain = "standalone_ps7_cortexa9_0")

platform = client.get_component(name="hello_world_platform")
status = platform.build()

comp = client.get_component(name="hello_world")
comp.build()

vitis.dispose()

