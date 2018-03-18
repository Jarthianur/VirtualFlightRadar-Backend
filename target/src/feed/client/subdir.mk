################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/feed/client/AprscClient.cpp \
../src/feed/client/Client.cpp \
../src/feed/client/GpsdClient.cpp \
../src/feed/client/SbsClient.cpp \
../src/feed/client/SensorClient.cpp 

OBJS += \
./src/feed/client/AprscClient.o \
./src/feed/client/Client.o \
./src/feed/client/GpsdClient.o \
./src/feed/client/SbsClient.o \
./src/feed/client/SensorClient.o 

CPP_DEPS += \
./src/feed/client/AprscClient.d \
./src/feed/client/Client.d \
./src/feed/client/GpsdClient.d \
./src/feed/client/SbsClient.d \
./src/feed/client/SensorClient.d 


# Each subdirectory must supply rules for building sources it contributes
src/feed/client/%.o: ../src/feed/client/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x $(BOOST_I) -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


