################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/aircraft/Aircraft.cpp \
../src/aircraft/AircraftContainer.cpp \
../src/aircraft/AircraftProcessor.cpp 

OBJS += \
./src/aircraft/Aircraft.o \
./src/aircraft/AircraftContainer.o \
./src/aircraft/AircraftProcessor.o 

CPP_DEPS += \
./src/aircraft/Aircraft.d \
./src/aircraft/AircraftContainer.d \
./src/aircraft/AircraftProcessor.d 


# Each subdirectory must supply rules for building sources it contributes
src/aircraft/%.o: ../src/aircraft/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


