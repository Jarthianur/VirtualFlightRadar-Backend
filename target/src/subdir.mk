################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Main.cpp \
../src/VFRB.cpp \
../src/Logger.cpp

OBJS += \
./src/Main.o \
./src/VFRB.o \
./src/Logger.o

CPP_DEPS += \
./src/Main.d \
./src/VFRB.d \
./src/Logger.d

V_VERSION := ${VFRB_VERSION}

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@echo '$(V_VERSION)'
	$(CXX) -std=c++0x $(BOOST_I) -DVERSION=\"$(V_VERSION)\" -O$(OPT_LVL) $(DBG) -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


