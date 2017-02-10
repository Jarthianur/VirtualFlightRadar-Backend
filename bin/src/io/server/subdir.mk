################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/io/server/Connection.cpp \
../src/io/server/NMEAServer.cpp 

OBJS += \
./src/io/server/Connection.o \
./src/io/server/NMEAServer.o 

CPP_DEPS += \
./src/io/server/Connection.d \
./src/io/server/NMEAServer.d 


# Each subdirectory must supply rules for building sources it contributes
src/io/server/%.o: ../src/io/server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


