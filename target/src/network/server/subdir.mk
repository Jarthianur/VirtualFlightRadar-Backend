################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/network/server/Connection.cpp \
../src/network/server/Server.cpp 

OBJS += \
./src/network/server/Connection.o \
./src/network/server/Server.o 

CPP_DEPS += \
./src/network/server/Connection.d \
./src/network/server/Server.d 


# Each subdirectory must supply rules for building sources it contributes
src/network/server/%.o: ../src/network/server/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


