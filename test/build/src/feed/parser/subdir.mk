################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/feed/parser/AprsParser.cpp \
../../src/feed/parser/GpsParser.cpp \
../../src/feed/parser/SbsParser.cpp \
../../src/feed/parser/SensorParser.cpp 

OBJS += \
./src/feed/parser/AprsParser.o \
./src/feed/parser/GpsParser.o \
./src/feed/parser/SbsParser.o \
./src/feed/parser/SensorParser.o 

CPP_DEPS += \
./src/feed/parser/AprsParser.d \
./src/feed/parser/GpsParser.d \
./src/feed/parser/SbsParser.d \
./src/feed/parser/SensorParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/feed/parser/%.o: ../../src/feed/parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++1y $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


