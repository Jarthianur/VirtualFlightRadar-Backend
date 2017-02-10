################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/connection/server/Connection.cpp \
../src/connection/server/NMEAServer.cpp 

OBJS += \
./src/connection/server/Connection.o \
./src/connection/server/NMEAServer.o 

CPP_DEPS += \
./src/connection/server/Connection.d \
./src/connection/server/NMEAServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/connection/server/%.o: ../src/connection/server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


