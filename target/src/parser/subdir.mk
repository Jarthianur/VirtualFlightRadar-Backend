################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/parser/AprsParser.cpp \
../src/parser/GpsParser.cpp \
../src/parser/Parser.cpp \
../src/parser/SbsParser.cpp \
../src/parser/SensorParser.cpp 

OBJS += \
./src/parser/AprsParser.o \
./src/parser/GpsParser.o \
./src/parser/Parser.o \
./src/parser/SbsParser.o \
./src/parser/SensorParser.o 

CPP_DEPS += \
./src/parser/AprsParser.d \
./src/parser/GpsParser.d \
./src/parser/Parser.d \
./src/parser/SbsParser.d \
./src/parser/SensorParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/parser/%.o: ../src/parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


