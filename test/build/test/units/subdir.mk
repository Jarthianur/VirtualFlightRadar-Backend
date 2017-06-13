################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/units/TestAircraft.cpp \
../test/units/TestConfig.cpp \
../test/units/TestData.cpp \
../test/units/TestParser.cpp \
../test/units/TestUtil.cpp 

OBJS += \
./test/units/TestAircraft.o \
./test/units/TestConfig.o \
./test/units/TestData.o \
./test/units/TestParser.o \
./test/units/TestUtil.o 

CPP_DEPS += \
./test/units/TestAircraft.d \
./test/units/TestConfig.d \
./test/units/TestData.d \
./test/units/TestParser.d \
./test/units/TestUtil.d 


# Each subdirectory must supply rules for building sources it contributes
test/units/%.o: ../test/units/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


