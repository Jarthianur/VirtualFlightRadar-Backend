################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/base/Configuration.cpp \
../src/base/VFRB.cpp \
../src/base/WeatherFeed.cpp 

OBJS += \
./src/base/Configuration.o \
./src/base/VFRB.o \
./src/base/WeatherFeed.o 

CPP_DEPS += \
./src/base/Configuration.d \
./src/base/VFRB.d \
./src/base/WeatherFeed.d 


# Each subdirectory must supply rules for building sources it contributes
src/base/%.o: ../src/base/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


