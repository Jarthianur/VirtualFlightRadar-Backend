################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/data/processor/GpsProcessor.cpp \
../src/data/processor/AircraftProcessor.cpp

OBJS += \
./src/data/processor/GpsProcessor.o \
./src/data/processor/AircraftProcessor.o

CPP_DEPS += \
./src/data/processor/GpsProcessor.d \
./src/data/processor/AircraftProcessor.d


# Each subdirectory must supply rules for building sources it contributes
src/data/processor/%.o: ../src/data/processor/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


