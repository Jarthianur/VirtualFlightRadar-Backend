################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf
OPT_LVL := 1
# -g3
DBG :=
TARGET := ${VFRB_TARGET}
CXX := ${VFRB_COMPILER}
BOOST_L := ${BOOST_LIBS_L}
BOOST_I := ${BOOST_ROOT_I}

# All of the sources participating in the build are defined here
-include sources.mk
-include src/vfrb/subdir.mk
-include src/util/subdir.mk
-include src/tcp/server/subdir.mk
-include src/tcp/client/subdir.mk
-include src/parser/subdir.mk
-include src/data/subdir.mk
-include src/config/subdir.mk
-include src/aircraft/subdir.mk
-include src/subdir.mk
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

# All Target
all: VirtualFlightRadar-Backend

# Tool invocations
VirtualFlightRadar-Backend: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: G++ Linker'
	$(CXX) $(BOOST_L) -o $(TARGET) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS)
	-@echo ' '
	
.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
