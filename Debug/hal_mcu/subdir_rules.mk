################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
hal_mcu/hal_mcu.obj: ../hal_mcu/hal_mcu.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"D:/Program Files/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.4.LTS/bin/cl430" -vmsp -O2 --opt_for_speed=0 --use_hw_mpy=none --include_path="D:/Program Files/ti/ccsv7/ccs_base/msp430/include" --include_path="D:/Dropbox/RYA2017/30_Firmware/RC_RX/RYA2017_Receiver/PPM" --include_path="D:/Program Files/ti/ccsv7/tools/compiler/ti-cgt-msp430_16.9.4.LTS/include" --include_path="D:/Dropbox/RYA2017/30_Firmware/RC_RX/RYA2017_Receiver/hal_mcu" --include_path="D:/Dropbox/RYA2017/30_Firmware/RC_RX/RYA2017_Receiver/radio_drv" --include_path="D:/Dropbox/RYA2017/30_Firmware/RC_RX/RYA2017_Receiver/radio_drv/cc1101_drv" --include_path="D:/Dropbox/RYA2017/30_Firmware/RC_RX/RYA2017_Receiver/ADC" --define=__MSP430G2553__ --define=ENABLE_RANGE_EXTENDER -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="hal_mcu/hal_mcu.d" --obj_directory="hal_mcu" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


