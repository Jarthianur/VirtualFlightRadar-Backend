################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/data/processor/GpsProcessor.cpp \
../../src/data/processor/AircraftProcessor.cpp

OBJS += \
./src/data/processor/GpsProcessor.o \
./src/data/processor/AircraftProcessor.o

CPP_DEPS += \
./src/data/processor/GpsProcessor.d \
./src/data/processor/AircraftProcessor.d


# Each subdirectory must supply rules for building sources it contributes
src/data/processor/%.o: ../../src/data/processor/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++1y $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


