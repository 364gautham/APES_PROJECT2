################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/Source/portable/CCS/ARM_Cortex-R4/port.obj: ../FreeRTOS/Source/portable/CCS/ARM_Cortex-R4/port.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/gautham/workspace_v8/proj_2" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178" --include_path="/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/Source/portable/CCS/ARM_Cortex-R4/port.d_raw" --obj_directory="FreeRTOS/Source/portable/CCS/ARM_Cortex-R4" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

FreeRTOS/Source/portable/CCS/ARM_Cortex-R4/portASM.obj: ../FreeRTOS/Source/portable/CCS/ARM_Cortex-R4/portASM.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/gautham/workspace_v8/proj_2" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178" --include_path="/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/Source/portable/CCS/ARM_Cortex-R4/portASM.d_raw" --obj_directory="FreeRTOS/Source/portable/CCS/ARM_Cortex-R4" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


