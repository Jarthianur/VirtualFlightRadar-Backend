################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Aircraft.cpp \
../src/AircraftContainer.cpp \
../src/AircraftProcessor.cpp \
../src/ConfigReader.cpp \
../src/Configuration.cpp \
../src/ConnectIn.cpp \
../src/ConnectInExt.cpp \
../src/ConnectOutNMEA.cpp \
../src/Connection.cpp \
../src/Logger.cpp \
../src/Main.cpp \
../src/Math.cpp \
../src/Parser.cpp \
../src/ParserAPRS.cpp \
../src/ParserSBS.cpp \
../src/VFRB.cpp \
../src/WeatherFeed.cpp 

OBJS += \
./src/Aircraft.o \
./src/AircraftContainer.o \
./src/AircraftProcessor.o \
./src/ConfigReader.o \
./src/Configuration.o \
./src/ConnectIn.o \
./src/ConnectInExt.o \
./src/ConnectOutNMEA.o \
./src/Connection.o \
./src/Logger.o \
./src/Main.o \
./src/Math.o \
./src/Parser.o \
./src/ParserAPRS.o \
./src/ParserSBS.o \
./src/VFRB.o \
./src/WeatherFeed.o 

CPP_DEPS += \
./src/Aircraft.d \
./src/AircraftContainer.d \
./src/AircraftProcessor.d \
./src/ConfigReader.d \
./src/Configuration.d \
./src/ConnectIn.d \
./src/ConnectInExt.d \
./src/ConnectOutNMEA.d \
./src/Connection.d \
./src/Logger.d \
./src/Main.d \
./src/Math.d \
./src/Parser.d \
./src/ParserAPRS.d \
./src/ParserSBS.d \
./src/VFRB.d \
./src/WeatherFeed.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: G++ Compiler'
	$(CXX) -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


