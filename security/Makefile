#
# Makefile for the kernel security code
#

obj-$(CONFIG_KEYS)			+= keys/
subdir-$(CONFIG_SECURITY_SELINUX)	+= selinux
subdir-$(CONFIG_SECURITY_SMACK)		+= smack
subdir-$(CONFIG_SECURITY_TOMOYO)        += tomoyo
subdir-$(CONFIG_SECURITY_APPARMOR)	+= apparmor
subdir-$(CONFIG_SECURITY_YAMA)		+= yama
subdir-$(CONFIG_SECURITY_LOADPIN)	+= loadpin
subdir-$(CONFIG_ARCH_QCOM)		+= pfe
subdir-$(CONFIG_SECURITY_DSMS)		+= samsung/dsms

# always enable default capabilities
obj-y					+= commoncap.o
obj-$(CONFIG_MMU)			+= min_addr.o

# Object file lists
obj-$(CONFIG_SECURITY)			+= security.o
obj-$(CONFIG_SECURITYFS)		+= inode.o
obj-$(CONFIG_SECURITY_SELINUX)		+= selinux/
obj-$(CONFIG_SECURITY_SMACK)		+= smack/
obj-$(CONFIG_AUDIT)			+= lsm_audit.o
obj-$(CONFIG_SECURITY_TOMOYO)		+= tomoyo/
obj-$(CONFIG_SECURITY_APPARMOR)		+= apparmor/
obj-$(CONFIG_SECURITY_YAMA)		+= yama/
obj-$(CONFIG_SECURITY_LOADPIN)		+= loadpin/
obj-$(CONFIG_ARCH_QCOM)			+= pfe/
obj-$(CONFIG_CGROUP_DEVICE)		+= device_cgroup.o
obj-$(CONFIG_SECURITY_DSMS)		+= samsung/dsms/

# Object integrity file lists
subdir-$(CONFIG_INTEGRITY)		+= integrity
obj-$(CONFIG_INTEGRITY)			+= integrity/

# FIVE
subdir-$(CONFIG_FIVE)			+= samsung/five
obj-$(CONFIG_FIVE)			+= samsung/five/

# PROCA module
subdir-$(CONFIG_PROCA)			+= proca
obj-$(CONFIG_PROCA)			+= proca/

# Kbuild should always enter proca/gaf because gaf should be always built
subdir-y += proca/gaf
obj-y    += proca/gaf/

# TIMA uevent
obj-$(CONFIG_TIMA_UEVENT)     += tima_uevent/

# Knox SDP
ifndef CONFIG_NO_SDP_BASE
obj-y += sdp/
obj-y += sdp/built-in.o
endif

# MST driver
obj-$(CONFIG_MST_LDO)	  += mstdrv/

#TZ ICCC
obj-$(CONFIG_TZ_ICCC)     += tz_iccc/
