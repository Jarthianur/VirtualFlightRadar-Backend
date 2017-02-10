################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/io/client/APRSCClient.cpp \
../src/io/client/Client.cpp \
../src/io/client/SBSClient.cpp \
../src/io/client/WindClient.cpp 

OBJS += \
./src/io/client/APRSCClient.o \
./src/io/client/Client.o \
./src/io/client/SBSClient.o \
./src/io/client/WindClient.o 

CPP_DEPS += \
./src/io/client/APRSCClient.d \
./src/io/client/Client.d \
./src/io/client/SBSClient.d \
./src/io/client/WindClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/io/client/%.o: ../src/io/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


