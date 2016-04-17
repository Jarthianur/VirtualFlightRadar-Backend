################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Aircraft.cpp \
../src/Connector.cpp \
../src/ConnectorADSB.cpp \
../src/ExtendedAircraft.cpp \
../src/Main.cpp \
../src/Parser.cpp \
../src/ParserADSB.cpp 

OBJS += \
./src/Aircraft.o \
./src/Connector.o \
./src/ConnectorADSB.o \
./src/ExtendedAircraft.o \
./src/Main.o \
./src/Parser.o \
./src/ParserADSB.o 

CPP_DEPS += \
./src/Aircraft.d \
./src/Connector.d \
./src/ConnectorADSB.d \
./src/ExtendedAircraft.d \
./src/Main.d \
./src/Parser.d \
./src/ParserADSB.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


