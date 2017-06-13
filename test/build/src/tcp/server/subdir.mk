################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tcp/server/Connection.cpp \
../src/tcp/server/Server.cpp 

OBJS += \
./src/tcp/server/Connection.o \
./src/tcp/server/Server.o 

CPP_DEPS += \
./src/tcp/server/Connection.d \
./src/tcp/server/Server.d 


# Each subdirectory must supply rules for building sources it contributes
src/tcp/server/%.o: ../src/tcp/server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -Wall -c -fmessage-length=0 -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


