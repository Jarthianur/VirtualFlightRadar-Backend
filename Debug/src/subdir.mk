################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ADSBin.cpp \
../src/ADSBparser.cpp \
../src/Aircraft.cpp \
../src/Main.cpp 

OBJS += \
./src/ADSBin.o \
./src/ADSBparser.o \
./src/Aircraft.o \
./src/Main.o 

CPP_DEPS += \
./src/ADSBin.d \
./src/ADSBparser.d \
./src/Aircraft.d \
./src/Main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


