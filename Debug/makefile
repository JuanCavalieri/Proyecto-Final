################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/c6000_7.4.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./Sources/sdmm.obj" \
"./Sources/main.obj" \
"./Sources/ff.obj" \
"./Sources/dsk6713_led.obj" \
"./Sources/dsk6713_dip.obj" \
"./Sources/dsk6713_aic23_setfreq.obj" \
"./Sources/dsk6713_aic23_registers.obj" \
"./Sources/dsk6713_aic23_opencodec.obj" \
"./Sources/dsk6713.obj" \
"./Sources/SD.obj" \
"./Sources/Inicializaciones.obj" \
"./Sources/Codec.obj" \
"./Vectors_intr.obj" \
"../C6713dsk.cmd" \
-l"C:\DSK6713\c6000\bios\lib\csl6713.lib" \
-l"C:\DSK6713\c6000\dsk6713\lib\dsk6713bsl.lib" \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Sources/subdir_vars.mk
-include subdir_rules.mk
-include Sources/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Proyecto_DSK.out

# Tool invocations
Proyecto_DSK.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far -z -m"Proyecto_DSK.map" -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/lib" -i"C:/DSK6713/c6000/bios/lib" -i"C:/DSK6713/c6000/dsk6713/lib" -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="Proyecto_DSK_linkInfo.xml" --rom_model -o "Proyecto_DSK.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C6000_EXECUTABLE_OUTPUTS__QUOTED) "Proyecto_DSK.out"
	-$(RM) "Sources\Codec.pp" "Sources\Inicializaciones.pp" "Sources\SD.pp" "Sources\dsk6713.pp" "Sources\dsk6713_aic23_opencodec.pp" "Sources\dsk6713_aic23_registers.pp" "Sources\dsk6713_aic23_setfreq.pp" "Sources\dsk6713_dip.pp" "Sources\dsk6713_led.pp" "Sources\ff.pp" "Sources\main.pp" "Sources\sdmm.pp" 
	-$(RM) "Vectors_intr.obj" "Sources\Codec.obj" "Sources\Inicializaciones.obj" "Sources\SD.obj" "Sources\dsk6713.obj" "Sources\dsk6713_aic23_opencodec.obj" "Sources\dsk6713_aic23_registers.obj" "Sources\dsk6713_aic23_setfreq.obj" "Sources\dsk6713_dip.obj" "Sources\dsk6713_led.obj" "Sources\ff.obj" "Sources\main.obj" "Sources\sdmm.obj" 
	-$(RM) "Vectors_intr.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

