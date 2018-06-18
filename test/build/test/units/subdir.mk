################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../units/TestConfig.cpp \
../units/TestData.cpp \
../units/TestDataProcessor.cpp \
../units/TestFeedParser.cpp \
../units/TestMath.cpp \
../units/TestObject.cpp

OBJS += \
./test/units/TestConfig.o \
./test/units/TestData.o \
./test/units/TestDataProcessor.o \
./test/units/TestFeedParser.o \
./test/units/TestMath.o \
./test/units/TestObject.o

CPP_DEPS += \
./test/units/TestConfig.d \
./test/units/TestData.d \
./test/units/TestDataProcessor.d \
./test/units/TestFeedParser.d \
./test/units/TestMath.d \
./test/units/TestObject.d


# Each subdirectory must supply rules for building sources it contributes
test/units/%.o: ../units/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++1y $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


