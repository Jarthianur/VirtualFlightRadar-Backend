################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tcp/client/AprscClient.cpp \
../src/tcp/client/Client.cpp \
../src/tcp/client/GpsdClient.cpp \
../src/tcp/client/SbsClient.cpp \
../src/tcp/client/SensorClient.cpp 

OBJS += \
./src/tcp/client/AprscClient.o \
./src/tcp/client/Client.o \
./src/tcp/client/GpsdClient.o \
./src/tcp/client/SbsClient.o \
./src/tcp/client/SensorClient.o 

CPP_DEPS += \
./src/tcp/client/AprscClient.d \
./src/tcp/client/Client.d \
./src/tcp/client/GpsdClient.d \
./src/tcp/client/SbsClient.d \
./src/tcp/client/SensorClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/tcp/client/%.o: ../src/tcp/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


