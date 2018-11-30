################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/object/Aircraft.cpp \
../src/object/Atmosphere.cpp \
../src/object/Object.cpp \
../src/object/GpsPosition.cpp \
../src/object/Wind.cpp \
../src/object/DateTimeImplBoost.cpp

OBJS += \
./src/object/Aircraft.o \
./src/object/Atmosphere.o \
./src/object/Object.o \
./src/object/GpsPosition.o \
./src/object/Wind.o \
./src/object/DateTimeImplBoost.o

CPP_DEPS += \
./src/object/Aircraft.d \
./src/object/Atmosphere.d \
./src/object/Object.d \
./src/object/GpsPosition.d \
./src/object/Wind.d \
./src/object/DateTimeImplBoost.d

# Each subdirectory must supply rules for building sources it contributes
src/object/%.o: ../src/object/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++14 $(BOOST_I) -I"../include" -I"../include/object" -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


