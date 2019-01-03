################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Sources/Funciones.obj: ../Sources/Funciones.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/Funciones.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Inicializaciones.obj: ../Sources/Inicializaciones.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/Inicializaciones.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/dsk6713.obj: ../Sources/dsk6713.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/dsk6713.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/dsk6713_aic23_opencodec.obj: ../Sources/dsk6713_aic23_opencodec.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/dsk6713_aic23_opencodec.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/dsk6713_aic23_registers.obj: ../Sources/dsk6713_aic23_registers.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/dsk6713_aic23_registers.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/dsk6713_aic23_setfreq.obj: ../Sources/dsk6713_aic23_setfreq.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/dsk6713_aic23_setfreq.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/dsk6713_dip.obj: ../Sources/dsk6713_dip.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/dsk6713_dip.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/dsk6713_led.obj: ../Sources/dsk6713_led.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/dsk6713_led.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ff.obj: ../Sources/ff.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/ff.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/main.obj: ../Sources/main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/main.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Sources/sdmm.obj: ../Sources/sdmm.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O3 -g --include_path="C:/C6xCSL/include" --include_path="E:/Facultad/Proyecto Final/Proyecto_DSK/Headers" --include_path="C:/DSK6713/c6000/dsk6713/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --define=CHIP_6713 --display_error_number --diag_warning=225 --diag_wrap=off --mem_model:data=far --preproc_with_compile --preproc_dependency="Sources/sdmm.pp" --obj_directory="Sources" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


