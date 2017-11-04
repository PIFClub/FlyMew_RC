################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
radio_drv/cc112x_drv/cc112x_drv.obj: ../radio_drv/cc112x_drv/cc112x_drv.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/bin/cl430" -vmsp -O2 --opt_for_speed=0 --use_hw_mpy=none --include_path="C:/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/include" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/hal_mcu" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/uart_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv/cc1101_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv/cc112x_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv/cc1190_drv" --advice:power="all" -g --define=__MSP430G2553__ --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --printf_support=minimal --preproc_with_compile --preproc_dependency="radio_drv/cc112x_drv/cc112x_drv.d" --obj_directory="radio_drv/cc112x_drv" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

radio_drv/cc112x_drv/cc112x_utils.obj: ../radio_drv/cc112x_drv/cc112x_utils.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/bin/cl430" -vmsp -O2 --opt_for_speed=0 --use_hw_mpy=none --include_path="C:/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/ti/ccsv6/tools/compiler/msp430_15.12.3.LTS/include" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/hal_mcu" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/uart_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv/cc1101_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv/cc112x_drv" --include_path="F:/NHH_Works/Dropbox/MyProjects/workspace_v5_1/LaunchPad_trx_demo/radio_drv/cc1190_drv" --advice:power="all" -g --define=__MSP430G2553__ --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --printf_support=minimal --preproc_with_compile --preproc_dependency="radio_drv/cc112x_drv/cc112x_utils.d" --obj_directory="radio_drv/cc112x_drv" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


