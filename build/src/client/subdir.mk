################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/client/ConnectorImplBoost.cpp \
../src/client/Client.cpp \
../src/client/AprscClient.cpp \
../src/client/GpsdClient.cpp \
../src/client/SbsClient.cpp \
../src/client/SensorClient.cpp \
../src/client/ClientFactory.cpp \
../src/client/ClientManager.cpp

OBJS += \
./src/client/ConnectorImplBoost.o \
./src/client/Client.o \
./src/client/AprscClient.o \
./src/client/GpsdClient.o \
./src/client/SbsClient.o \
./src/client/SensorClient.o \
./src/client/ClientFactory.o \
./src/client/ClientManager.o

CPP_DEPS += \
./src/client/ConnectorImplBoost.d \
./src/client/Client.d \
./src/client/AprscClient.d \
./src/client/GpsdClient.d \
./src/client/SbsClient.d \
./src/client/SensorClient.d \
./src/client/ClientFactory.d \
./src/client/ClientManager.d


# Each subdirectory must supply rules for building sources it contributes
src/client/%.o: ../src/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++14 $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


