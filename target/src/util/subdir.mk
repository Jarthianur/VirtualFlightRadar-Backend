################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/util/GPSmodule.cpp \
../src/util/Logger.cpp \
../src/util/Math.cpp 

OBJS += \
./src/util/GPSmodule.o \
./src/util/Logger.o \
./src/util/Math.o 

CPP_DEPS += \
./src/util/GPSmodule.d \
./src/util/Logger.d \
./src/util/Math.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


