################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/config/ConfigReader.cpp \
../../src/config/Configuration.cpp \
../../src/config/Properties.cpp 

OBJS += \
./src/config/ConfigReader.o \
./src/config/Configuration.o \
./src/config/Properties.o 

CPP_DEPS += \
./src/config/ConfigReader.d \
./src/config/Configuration.d \
./src/config/Properties.d 


# Each subdirectory must supply rules for building sources it contributes
src/config/%.o: ../../src/config/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++14 $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


