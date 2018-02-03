################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/network/server/Connection.cpp \
../../src/network/server/Server.cpp 

OBJS += \
./src/network/server/Connection.o \
./src/network/server/Server.o 

CPP_DEPS += \
./src/network/server/Connection.d \
./src/network/server/Server.d 


# Each subdirectory must supply rules for building sources it contributes
src/network/server/%.o: ../../src/network/server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


