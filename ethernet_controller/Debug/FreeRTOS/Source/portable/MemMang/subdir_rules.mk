################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/Source/portable/MemMang/heap_5.obj: ../FreeRTOS/Source/portable/MemMang/heap_5.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/Users/gautham/workspace_v8/proj_2" --include_path="/Users/gautham/Documents/APES_project1/" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178/third_party/exosite/" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178/examples/boards/ek-tm4c1294xl/drivers" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178/examples/boards/ek-tm4c1294xl/drivers/" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178/inc/" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/include/" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS/Source/portable/CCS/ARM_CM4F/" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178/driverlib" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178/utils/" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS/Source/include" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS/Source/include/" --include_path="/Users/gautham/workspace_v8/proj_2/FreeRTOS" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP" --include_path="/Applications/ti/ccsv8/FreeRTOSv10.0.1/FreeRTOS-Plus/Source/FreeRTOS-Plus-TCP/portable/Compiler/GCC/" --include_path="/Applications/ti/ccsv8/SW-TM4C-2.1.4.178/" --include_path="/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.1.LTS/include" --define=ccs="ccs" --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/Source/portable/MemMang/heap_5.d_raw" --obj_directory="FreeRTOS/Source/portable/MemMang" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

