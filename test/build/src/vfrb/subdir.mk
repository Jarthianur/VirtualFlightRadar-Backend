################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/vfrb/Feed.cpp \
../src/vfrb/VFRB.cpp 

OBJS += \
./src/vfrb/Feed.o \
./src/vfrb/VFRB.o 

CPP_DEPS += \
./src/vfrb/Feed.d \
./src/vfrb/VFRB.d 


# Each subdirectory must supply rules for building sources it contributes
src/vfrb/%.o: ../../src/vfrb/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


