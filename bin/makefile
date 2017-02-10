################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf
NAME := vfrb
VERSION := _v2-0-0-SNAPSHOT
# path to boost libs prefixed by -L
# example: -L/home/user/opt/boost_1_63_0/stage/lib/
BOOST_LIBS := 

# All of the sources participating in the build are defined here
-include sources.mk
#-include test/subdir.mk
-include src/util/subdir.mk
-include src/parser/subdir.mk
-include src/main/subdir.mk
-include src/io/server/subdir.mk
-include src/io/reader/subdir.mk
-include src/io/logger/subdir.mk
-include src/io/client/subdir.mk
-include src/aircraft/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

#TEST_OBJS += $(filter-out ./src/Main.o ./src/ConfigReader.o,$(OBJS))

# Tool invocations
VirtualFlightRadar-Backend: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: G++ Linker'
	$(CXX) $(BOOST_LIBS) -pthread -o $(NAME)$(VERSION) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	
# All Target
all: VirtualFlightRadar-Backend #tests	
	
# Test
#tests: $(TEST_OBJS) $(USER_OBJS)
	#-@echo 'Building target: $@'
	#-@echo 'Invoking: G++ Linker'
	#-$(CXX) -pthread -o $(NAME)$(VERSION)_TEST $(TEST_OBJS) $(USER_OBJS) $(LIBS)
	#-@echo 'Finished building target: $@'
	#-@echo ' '

# Other Targets ./test/UnitTests.o $(NAME)$(VERSION)_TEST
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) $(NAME)$(VERSION)
	-@echo ' '
	
.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
