################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf
OPT_LVL := $(or ${VFRB_OPT},${VFRB_OPT},3)
DBG := ${VFRB_DEBUG}
TARGET := $(or ${VFRB_TARGET},${VFRB_TARGET},vfrb)
CXX := $(or ${VFRB_COMPILER},${VFRB_COMPILER},g++)
BOOST_L := ${BOOST_LIBS_L}
BOOST_I := ${BOOST_ROOT_I}

# All of the sources participating in the build are defined here
-include sources.mk
-include src/server/subdir.mk
-include src/object/subdir.mk
-include src/feed/parser/subdir.mk
-include src/client/subdir.mk
-include src/feed/subdir.mk
-include src/data/processor/subdir.mk
-include src/data/subdir.mk
-include src/config/subdir.mk
-include src/util/subdir.mk
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
	$(CXX) $(BOOST_L) $(DBG) -o $(TARGET) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS)
	-@echo ' '
	
.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
