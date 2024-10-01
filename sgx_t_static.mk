######## Intel(R) SGX SDK Settings ########
SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= SIM
SGX_ARCH ?= x64
TRUSTED_DIR=static_trusted

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_CFLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_CFLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
        SGX_COMMON_CFLAGS += -O0 -g
else
        SGX_COMMON_CFLAGS += -O2
endif

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif

Crypto_Library_Name := sgx_tcrypto

untrusted_Shim_Files := $(wildcard untrusted/*.c)

Shim_Cpp_Files := $(TRUSTED_DIR)/shim.cpp $(wildcard static_trusted/*.cpp)
Shim_C_Files := $(wildcard static_trusted/*.c)
Shim_Include_Paths := -IInclude -IShim -Istatic_trusted -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/libcxx

Flags_Just_For_C := -Wno-implicit-function-declaration -std=c11
Common_C_Cpp_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector $(Shim_Include_Paths) -fno-builtin-printf -I.
Shim_C_Flags := $(Flags_Just_For_C) $(Common_C_Cpp_Flags)
Shim_Cpp_Flags := $(Common_C_Cpp_Flags) -std=c++17 -nostdinc++ -fno-builtin-printf  -I.

Shim_Cpp_Flags := $(Shim_Cpp_Flags)  -fno-builtin-printf

Shim_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -lsgx_pthread -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 \
	-Wl,--version-script=trusted/shim.lds


Shim_Cpp_Objects := $(Shim_Cpp_Files:.cpp=.o)
Shim_C_Objects := $(Shim_C_Files:.c=.o)

untrusted_Shim_Objects := $(untrusted_Shim_Files:.c=.o)

ifeq ($(SGX_MODE), HW)
ifneq ($(SGX_DEBUG), 1)
ifneq ($(SGX_PRERELEASE), 1)
Build_Mode = HW_RELEASE
endif
endif
endif


.PHONY: all run

all: libshim.sgx.static.lib.a

######## shim Objects ########

$(TRUSTED_DIR)/shim_t.h: $(SGX_EDGER8R) ./$(TRUSTED_DIR)/shim.edl
	@cd ./$(TRUSTED_DIR) && $(SGX_EDGER8R) --header-only  --trusted ../$(TRUSTED_DIR)/shim.edl --search-path ../$(TRUSTED_DIR) --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

$(TRUSTED_DIR)/shim_t.o: ./trusted/shim_t.c
	@$(CC) $(Shim_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

$(TRUSTED_DIR)/%.o: $(TRUSTED_DIR)/%.cpp
	@$(CXX) $(Shim_Include_Paths) $(Shim_Cpp_Flags) $(Enclave_Link_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

$(TRUSTED_DIR)/%.o: $(TRUSTED_DIR)/%.c
	@$(CC) $(Shim_C_Flags) $(Enclave_Link_Flags) -c $< -o $@
	@echo "CC  <=  $<"

libshim.sgx.static.lib.a: $(TRUSTED_DIR)/shim_t.h $(Shim_Cpp_Objects) $(Shim_C_Objects)
	ar rcs libshim.sgx.static.lib.a $(Shim_Cpp_Objects) $(Shim_C_Objects)

clean:
	@rm -f libshim.* $(TRUSTED_DIR)/shim_t.* $(Shim_Cpp_Objects) $(Shim_C_Objects)