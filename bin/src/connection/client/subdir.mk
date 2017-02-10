################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/connection/client/APRSCClient.cpp \
../src/connection/client/Client.cpp \
../src/connection/client/SBSClient.cpp \
../src/connection/client/WindClient.cpp 

OBJS += \
./src/connection/client/APRSCClient.o \
./src/connection/client/Client.o \
./src/connection/client/SBSClient.o \
./src/connection/client/WindClient.o 

CPP_DEPS += \
./src/connection/client/APRSCClient.d \
./src/connection/client/Client.d \
./src/connection/client/SBSClient.d \
./src/connection/client/WindClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/connection/client/%.o: ../src/connection/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


