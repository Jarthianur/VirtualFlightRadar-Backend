################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/connection/ConnectIn.cpp \
../src/connection/ConnectInExt.cpp \
../src/connection/ConnectOutNMEA.cpp \
../src/connection/Connection.cpp \
../src/connection/ConnectionException.cpp \
../src/connection/InputConnection.cpp \
../src/connection/OutputConnection.cpp 

OBJS += \
./src/connection/ConnectIn.o \
./src/connection/ConnectInExt.o \
./src/connection/ConnectOutNMEA.o \
./src/connection/Connection.o \
./src/connection/ConnectionException.o \
./src/connection/InputConnection.o \
./src/connection/OutputConnection.o 

CPP_DEPS += \
./src/connection/ConnectIn.d \
./src/connection/ConnectInExt.d \
./src/connection/ConnectOutNMEA.d \
./src/connection/Connection.d \
./src/connection/ConnectionException.d \
./src/connection/InputConnection.d \
./src/connection/OutputConnection.d 


# Each subdirectory must supply rules for building sources it contributes
src/connection/%.o: ../src/connection/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


