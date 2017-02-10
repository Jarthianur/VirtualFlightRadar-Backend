################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/main/Main.cpp \
../src/main/VFRB.cpp 

OBJS += \
./src/main/Main.o \
./src/main/VFRB.o 

CPP_DEPS += \
./src/main/Main.d \
./src/main/VFRB.d 


# Each subdirectory must supply rules for building sources it contributes
src/main/%.o: ../src/main/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


