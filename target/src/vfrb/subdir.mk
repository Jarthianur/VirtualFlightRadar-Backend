################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/vfrb/VFRB.cpp 

OBJS += \
./src/vfrb/VFRB.o 

CPP_DEPS += \
./src/vfrb/VFRB.d 

# Each subdirectory must supply rules for building sources it contributes
src/vfrb/%.o: ../src/vfrb/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


