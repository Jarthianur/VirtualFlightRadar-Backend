################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/VFRB.cpp \
../../src/Signals.cpp \
../../src/Logger.cpp

OBJS += \
./src/VFRB.o \
./src/Signals.o \
./src/Logger.o

CPP_DEPS += \
./src/VFRB.d \
./src/Signals.d \
./src/Logger.d

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++1y $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


