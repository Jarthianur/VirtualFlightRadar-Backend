################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/data/GpsData.cpp \
../src/data/WindData.cpp \
../src/data/AtmosphereData.cpp 

OBJS += \
./src/data/GpsData.o \
./src/data/WindData.o \
./src/data/AtmosphereData.o 

CPP_DEPS += \
./src/data/GpsData.d \
./src/data/WindData.d \
./src/data/AtmosphereData.d 


# Each subdirectory must supply rules for building sources it contributes
src/data/%.o: ../src/data/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


