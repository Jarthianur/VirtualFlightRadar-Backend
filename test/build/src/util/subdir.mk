################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/util/Logger.cpp \
../../src/util/Signals.cpp

OBJS += \
./src/util/Logger.o \
./src/util/Signals.o 

CPP_DEPS += \
./src/util/Logger.d \
./src/util/Signals.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/%.o: ../../src/util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++14 $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

