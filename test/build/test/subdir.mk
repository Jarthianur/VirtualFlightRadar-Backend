################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../UnitTests.cpp \
../TestServer.cpp \
../TestObject.cpp \
../TestMath.cpp \
../TestFeedParser.cpp \
../TestFeed.cpp \
../TestDataProcessor.cpp \
../TestData.cpp \
../TestConfig.cpp \
../TestClient.cpp \
../TcpInterfaceImplTest.cpp \
../SocketImplTest.cpp \
../DateTimeImplTest.cpp

OBJS += \
./test/UnitTests.o \
./TestServer.o \
./TestObject.o \
./TestMath.o \
./TestFeedParser.o \
./TestFeed.o \
./TestDataProcessor.o \
./TestData.o \
./TestConfig.o \
./TestClient.o \
./TcpInterfaceImplTest.o \
./SocketImplTest.o \
./DateTimeImplTest.o

CPP_DEPS += \
./test/UnitTests.d \
./TestServer.d \
./TestObject.d \
./TestMath.d \
./TestFeedParser.d \
./TestFeed.d \
./TestDataProcessor.d \
./TestData.d \
./TestConfig.d \
./TestClient.d \
./TcpInterfaceImplTest.d \
./SocketImplTest.d \
./DateTimeImplTest.d


# Each subdirectory must supply rules for building sources it contributes
test/%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++1y $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


