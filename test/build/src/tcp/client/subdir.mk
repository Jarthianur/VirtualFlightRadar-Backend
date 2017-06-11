################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tcp/client/APRSCClient.cpp \
../src/tcp/client/Client.cpp \
../src/tcp/client/GPSDClient.cpp \
../src/tcp/client/SBSClient.cpp \
../src/tcp/client/SensorClient.cpp 

OBJS += \
./src/tcp/client/APRSCClient.o \
./src/tcp/client/Client.o \
./src/tcp/client/GPSDClient.o \
./src/tcp/client/SBSClient.o \
./src/tcp/client/SensorClient.o 

CPP_DEPS += \
./src/tcp/client/APRSCClient.d \
./src/tcp/client/Client.d \
./src/tcp/client/GPSDClient.d \
./src/tcp/client/SBSClient.d \
./src/tcp/client/SensorClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/tcp/client/%.o: ../../src/tcp/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


