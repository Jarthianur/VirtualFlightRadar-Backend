################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/io/reader/ConfigReader.cpp 

OBJS += \
./src/io/reader/ConfigReader.o 

CPP_DEPS += \
./src/io/reader/ConfigReader.d 


# Each subdirectory must supply rules for building sources it contributes
src/io/reader/%.o: ../src/io/reader/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


