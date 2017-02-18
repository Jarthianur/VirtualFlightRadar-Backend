################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tcp/client/APRSCClient.cpp \
../src/tcp/client/Client.cpp \
../src/tcp/client/SBSClient.cpp \
../src/tcp/client/WindClient.cpp 

OBJS += \
./src/tcp/client/APRSCClient.o \
./src/tcp/client/Client.o \
./src/tcp/client/SBSClient.o \
./src/tcp/client/WindClient.o 

CPP_DEPS += \
./src/tcp/client/APRSCClient.d \
./src/tcp/client/Client.d \
./src/tcp/client/SBSClient.d \
./src/tcp/client/WindClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/tcp/client/%.o: ../src/tcp/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


