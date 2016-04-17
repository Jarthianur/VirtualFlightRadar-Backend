################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ADSBParser.cpp \
../src/Aircraft.cpp \
../src/Connector.cpp \
../src/ConnectorADSB.cpp \
../src/Main.cpp 

OBJS += \
./src/ADSBParser.o \
./src/Aircraft.o \
./src/Connector.o \
./src/ConnectorADSB.o \
./src/Main.o 

CPP_DEPS += \
./src/ADSBParser.d \
./src/Aircraft.d \
./src/Connector.d \
./src/ConnectorADSB.d \
./src/Main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


