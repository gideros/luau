#-------------------------------------------------
#
# Project created by QtCreator 2011-12-13T12:06:49
#
#-------------------------------------------------

QT       -= core gui

TARGET = lua
TEMPLATE = lib
CONFIG   += silent c++17

win32 {
DEFINES += LUA_BUILD_AS_DLL LUA_CORE
}

//QMAKE_CXXFLAGS_DEBUG += -O2
QMAKE_CXXFLAGS_DEBUG += -DLUAU_ENABLE_ASSERT
DEFINES+= LUAU_ENABLE_CODEGEN

defineReplace(expand) {
    names = $$1
    prefix=$$2
    suffix=$$3
    expanded =

    for(name, names) {
        expanded+= $${prefix}$${name}$${suffix}
    }
    return ($$expanded)
}

INCLUDEPATH += \
	VM/src \
	VM/include \
	Common/include \
        Compiler/include \
        CodeGen/include \
        Ast/include \
        ../libgvfs

SOURCES += \
         $$expand(lapi laux lbaselib lbitlib lbuffer lbuflib lbuiltins lcorolib ldblib ldebug ldo lfunc lgc lgcdebug linit lint64lib liolib lmathlib lmem lnumprint lobject loslib lperf lstate lstring lstrlib \
         ltable ltablib ltm ludata lutf8lib lveclib lvmexecute lvmload lvmutils,VM/src/,.cpp) \
         $$expand(Builtins BuiltinFolding BytecodeBuilder ConstantFolding Compiler CostModel lcode PseudoCode TableShape Types ValueTracking,Compiler/src/,.cpp) \
         $$expand(BytecodeAnalysis BytecodeSummary CodeAllocator CodeBlockUnwind CodeGen CodeGenAssembly CodeGenContext CodeGenUtils \
         IrAnalysis IrBuilder IrCallWrapperX64 IrDump IrTranslateBuiltins IrTranslation IrUtils IrValueLocationTracking \
         lcodegen NativeProtoExecData NativeState OptimizeConstProp OptimizeDeadStore SharedCodeAllocator \
         AssemblyBuilderX64 OptimizeFinalX64 EmitBuiltinsX64 EmitCommonX64 EmitInstructionX64 CodeGenX64 IrLoweringX64 IrRegAllocX64 UnwindBuilderWin \
         AssemblyBuilderA64 CodeGenA64 IrLoweringA64 IrRegAllocA64 UnwindBuilderDwarf2 \
         ,CodeGen/src/,.cpp) \
         $$expand(StringUtils TimeTrace,Common/src/,.cpp) \
         $$expand(Allocator Ast Confusables Cst Lexer Location Parser PrettyPrinter,Ast/src/,.cpp)

win32 {
LIBS += -L"../libgvfs/release" -lgvfs
}

macx {
LIBS += -L"../libgvfs" -lgvfs
}
else {

unix:!macx {
LIBS += ../libgvfs/libgvfs.so
}

}
