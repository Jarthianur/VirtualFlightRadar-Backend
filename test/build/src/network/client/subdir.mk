################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/network/client/AprscClient.cpp \
../../src/network/client/Client.cpp \
../../src/network/client/GpsdClient.cpp \
../../src/network/client/SbsClient.cpp \
../../src/network/client/SensorClient.cpp 

OBJS += \
./src/network/client/AprscClient.o \
./src/network/client/Client.o \
./src/network/client/GpsdClient.o \
./src/network/client/SbsClient.o \
./src/network/client/SensorClient.o 

CPP_DEPS += \
./src/network/client/AprscClient.d \
./src/network/client/Client.d \
./src/network/client/GpsdClient.d \
./src/network/client/SbsClient.d \
./src/network/client/SensorClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/network/client/%.o: ../../src/network/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


