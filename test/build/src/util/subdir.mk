################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/util/GpsModule.cpp \
../../src/util/Logger.cpp 

OBJS += \
./src/util/GpsModule.o \
./src/util/Logger.o 

CPP_DEPS += \
./src/util/GpsModule.d \
./src/util/Logger.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


