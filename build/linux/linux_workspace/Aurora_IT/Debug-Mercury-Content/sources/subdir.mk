################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/fware/work/Aurora/aurora/src/frw/az_main.c 

OBJS += \
./sources/az_main.o 

C_DEPS += \
./sources/az_main.d 


# Each subdirectory must supply rules for building sources it contributes
sources/az_main.o: /home/fware/work/Aurora/aurora/src/frw/az_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC ix86 64-bit Compiler'
	/usr/bin/gcc -DTATL_TRACE_ENABLE -Dlinux -DCONFIG_IX86_64 -I/opt/mercury/include -I/usr/include/asm-x86_64 -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys/linux" -I"/home/fware/work/Aurora/linux_workspace/../aurora/inc/sys" -O0 -g3 -w -Wall -c -fmessage-length=0 -m64 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


