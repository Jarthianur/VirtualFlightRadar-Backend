################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/data/AircraftContainer.cpp \
../../src/data/ClimateData.cpp 

OBJS += \
./src/data/AircraftContainer.o \
./src/data/ClimateData.o 

CPP_DEPS += \
./src/data/AircraftContainer.d \
./src/data/ClimateData.d 


# Each subdirectory must supply rules for building sources it contributes
src/data/%.o: ../../src/data/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


