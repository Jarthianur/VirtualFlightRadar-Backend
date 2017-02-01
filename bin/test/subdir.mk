################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../test/UnitTests.cpp

CPP_DEPS += \
./test/UnitTests.d

TEST_OBJS += \
./test/UnitTests.o

# Each subdirectory must supply rules for building sources it contributes

test/%.o: ../test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: G++ Compiler'
	$(CXX) -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

