################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Aircraft.cpp \
../src/ConnectIn.cpp \
../src/ConnectInADSB.cpp \
../src/ConnectInOGN.cpp \
../src/ConnectOutNMEA.cpp \
../src/ExtendedAircraft.cpp \
../src/Main.cpp \
../src/Parser.cpp \
../src/ParserADSB.cpp \
../src/ParserOGN.cpp \
../src/VFRB.cpp 

OBJS += \
./src/Aircraft.o \
./src/ConnectIn.o \
./src/ConnectInADSB.o \
./src/ConnectInOGN.o \
./src/ConnectOutNMEA.o \
./src/ExtendedAircraft.o \
./src/Main.o \
./src/Parser.o \
./src/ParserADSB.o \
./src/ParserOGN.o \
./src/VFRB.o 

CPP_DEPS += \
./src/Aircraft.d \
./src/ConnectIn.d \
./src/ConnectInADSB.d \
./src/ConnectInOGN.d \
./src/ConnectOutNMEA.d \
./src/ExtendedAircraft.d \
./src/Main.d \
./src/Parser.d \
./src/ParserADSB.d \
./src/ParserOGN.d \
./src/VFRB.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


