################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../src/feed/AprscFeed.cpp \
../../src/feed/Feed.cpp \
../../src/feed/GpsFeed.cpp \
../../src/feed/SbsFeed.cpp \
../../src/feed/SensorFeed.cpp \
../../src/feed/FeedFactory.cpp

OBJS += \
./src/feed/AprscFeed.o \
./src/feed/Feed.o \
./src/feed/GpsFeed.o \
./src/feed/SbsFeed.o \
./src/feed/SensorFeed.o \
./src/feed/FeedFactory.o

CPP_DEPS += \
./src/feed/AprscFeed.d \
./src/feed/Feed.d \
./src/feed/GpsFeed.d \
./src/feed/SbsFeed.d \
./src/feed/SensorFeed.d \
./src/feed/FeedFactory.d 


# Each subdirectory must supply rules for building sources it contributes
src/feed/%.o: ../../src/feed/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++1y $(BOOST_I) -O0 -g3 -Wall -c -fmessage-length=0 -fprofile-arcs -ftest-coverage -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


