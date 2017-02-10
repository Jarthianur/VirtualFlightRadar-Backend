################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/parser/APRSParser.cpp \
../src/parser/Parser.cpp \
../src/parser/SBSParser.cpp \
../src/parser/WindParser.cpp 

OBJS += \
./src/parser/APRSParser.o \
./src/parser/Parser.o \
./src/parser/SBSParser.o \
./src/parser/WindParser.o 

CPP_DEPS += \
./src/parser/APRSParser.d \
./src/parser/Parser.d \
./src/parser/SBSParser.d \
./src/parser/WindParser.d 


# Each subdirectory must supply rules for building sources it contributes
src/parser/%.o: ../src/parser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/julian/opt/boost_1_63_0 -O0 -g3 -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


