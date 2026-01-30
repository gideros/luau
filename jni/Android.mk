LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE            := gvfs
LOCAL_SRC_FILES         := ../../libgvfs/libs/$(TARGET_ARCH_ABI)/libgvfs.so

include $(PREBUILT_SHARED_LIBRARY)

###

include $(CLEAR_VARS)

LOCAL_MODULE := lua
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -O2 -DLUA_USE_MKSTEMP -std=c++17
LOCAL_C_INCLUDES += $(addprefix $(LOCAL_PATH)/../,VM/src VM/include Common/Include Compiler/include Codegen/include Ast/include ../libgvfs)

LOCAL_SRC_FILES += $(addsuffix .cpp, \
        $(addprefix ../VM/src/,lapi laux lbaselib lbitlib lbuffer lbuflib lbuiltins lcorolib ldblib ldebug ldo lfunc lgc lgcdebug linit lint64lib liolib lmathlib lmem lnumprint lobject loslib lperf lstate lstring lstrlib \
         ltable ltablib ltm ludata lutf8lib lveclib lvmexecute lvmload lvmutils) \
        $(addprefix ../Compiler/src/,Builtins BuiltinFolding BytecodeBuilder ConstantFolding Compiler CostModel lcode PseudoCode TableShape Types ValueTracking) \
        $(addprefix ../Common/src/,StringUtils TimeTrace) \
        $(addprefix ../Ast/src/,Allocator Ast Confusables Cst Lexer Location Parser PrettyPrinter))

#CodeGen not yet support for android
LOCAL_CFLAGS += -DLUAU_ENABLE_CODEGEN
LOCAL_SRC_FILES += $(addsuffix .cpp, \
        $(addprefix ../CodeGen/src/,BytecodeAnalysis BytecodeSummary CodeAllocator CodeBlockUnwind CodeGen CodeGenAssembly CodeGenContext CodeGenUtils \
         IrAnalysis IrBuilder IrCallWrapperX64 IrDump IrTranslateBuiltins IrTranslation IrUtils IrValueLocationTracking \
         lcodegen NativeProtoExecData NativeState OptimizeConstProp OptimizeDeadStore SharedCodeAllocator \
         AssemblyBuilderX64 OptimizeFinalX64 EmitBuiltinsX64 EmitCommonX64 EmitInstructionX64 CodeGenX64 IrLoweringX64 IrRegAllocX64 UnwindBuilderWin \
         AssemblyBuilderA64 CodeGenA64 IrLoweringA64 IrRegAllocA64 UnwindBuilderDwarf2))

LOCAL_LDLIBS := -ldl
LOCAL_SHARED_LIBRARIES := gvfs

include $(BUILD_SHARED_LIBRARY)
