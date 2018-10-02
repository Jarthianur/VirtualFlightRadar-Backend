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
./test/TestServer.o \
./test/TestObject.o \
./test/TestMath.o \
./test/TestFeedParser.o \
./test/TestFeed.o \
./test/TestDataProcessor.o \
./test/TestData.o \
./test/TestConfig.o \
./test/TestClient.o \
./test/TcpInterfaceImplTest.o \
./test/SocketImplTest.o \
./test/DateTimeImplTest.o

CPP_DEPS += \
./test/UnitTests.d \
./test/TestServer.d \
./test/TestObject.d \
./test/TestMath.d \
./test/TestFeedParser.d \
./test/TestFeed.d \
./test/TestDataProcessor.d \
./test/TestData.d \
./test/TestConfig.d \
./test/TestClient.d \
./test/TcpInterfaceImplTest.d \
./test/SocketImplTest.d \
./test/DateTimeImplTest.d


# Each subdirectory must supply rules for building sources it contributes
test/%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++1y $(BOOST_I) -O0 -g3 -Wall -DSCTF_CUSTOM_EPSILON=0.000001 -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


