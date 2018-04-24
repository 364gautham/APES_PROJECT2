################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./adc.obj" \
"./aes.obj" \
"./can.obj" \
"./comp.obj" \
"./cpu.obj" \
"./crc.obj" \
"./des.obj" \
"./eeprom.obj" \
"./emac.obj" \
"./epi.obj" \
"./epi_workaround_ccs.obj" \
"./flash.obj" \
"./fpu.obj" \
"./gpio.obj" \
"./hibernate.obj" \
"./i2c.obj" \
"./interrupt.obj" \
"./lcd.obj" \
"./main.obj" \
"./mpu.obj" \
"./onewire.obj" \
"./pwm.obj" \
"./qei.obj" \
"./shamd5.obj" \
"./ssi.obj" \
"./sw_crc.obj" \
"./sysctl.obj" \
"./sysexc.obj" \
"./systick.obj" \
"./timer.obj" \
"./tm4c1294ncpdt_startup_ccs.obj" \
"./uart.obj" \
"./udma.obj" \
"./usb.obj" \
"./watchdog.obj" \
"./utils/uartstdio.obj" \
"./utils/ustdlib.obj" \
"../tm4c1294ncpdt.cmd" \
"../ccs/Debug/driverlib.lib" \
"../rvmdk/driverlib.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include ccs/Debug/subdir_vars.mk
-include ewarm/Exe/subdir_vars.mk
-include gcc/subdir_vars.mk
-include rvmdk/subdir_vars.mk
-include utils/subdir_vars.mk
-include subdir_rules.mk
-include ccs/Debug/subdir_rules.mk
-include ewarm/Exe/subdir_rules.mk
-include gcc/subdir_rules.mk
-include rvmdk/subdir_rules.mk
-include utils/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Gesture_sensor.out \

EXE_OUTPUTS__QUOTED += \
"Gesture_sensor.out" \

BIN_OUTPUTS += \
Gesture_sensor.hex \

BIN_OUTPUTS__QUOTED += \
"Gesture_sensor.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(A_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "Gesture_sensor.out"

# Tool invocations
Gesture_sensor.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(A_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --define=ccs="ccs" --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"Gesture_sensor.map" --heap_size=0 --stack_size=512 -i"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/lib" -i"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="Gesture_sensor_linkInfo.xml" --rom_model -o "Gesture_sensor.out" $(ORDERED_OBJS) $(A_SRCS)
	@echo 'Finished building target: $@'
	@echo ' '

Gesture_sensor.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.4.LTS/bin/armhex"  -o "Gesture_sensor.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "adc.obj" "aes.obj" "can.obj" "comp.obj" "cpu.obj" "crc.obj" "des.obj" "eeprom.obj" "emac.obj" "epi.obj" "epi_workaround_ccs.obj" "flash.obj" "fpu.obj" "gpio.obj" "hibernate.obj" "i2c.obj" "interrupt.obj" "lcd.obj" "main.obj" "mpu.obj" "onewire.obj" "pwm.obj" "qei.obj" "shamd5.obj" "ssi.obj" "sw_crc.obj" "sysctl.obj" "sysexc.obj" "systick.obj" "timer.obj" "tm4c1294ncpdt_startup_ccs.obj" "uart.obj" "udma.obj" "usb.obj" "watchdog.obj" "utils\uartstdio.obj" "utils\ustdlib.obj" 
	-$(RM) "adc.d" "aes.d" "can.d" "comp.d" "cpu.d" "crc.d" "des.d" "eeprom.d" "emac.d" "epi.d" "flash.d" "fpu.d" "gpio.d" "hibernate.d" "i2c.d" "interrupt.d" "lcd.d" "main.d" "mpu.d" "onewire.d" "pwm.d" "qei.d" "shamd5.d" "ssi.d" "sw_crc.d" "sysctl.d" "sysexc.d" "systick.d" "timer.d" "tm4c1294ncpdt_startup_ccs.d" "uart.d" "udma.d" "usb.d" "watchdog.d" "utils\uartstdio.d" "utils\ustdlib.d" 
	-$(RM) "epi_workaround_ccs.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

