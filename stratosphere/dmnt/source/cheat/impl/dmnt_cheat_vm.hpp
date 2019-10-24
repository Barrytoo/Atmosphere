/*
 * Copyright (c) 2018-2019 Atmosphère-NX
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdarg.h>
#include <stratosphere.hpp>
#include <stratosphere/dmnt.hpp>

namespace ams::dmnt::cheat::impl {

    enum CheatVmOpcodeType : u32 {
        CheatVmOpcodeType_StoreStatic = 0,
        CheatVmOpcodeType_BeginConditionalBlock = 1,
        CheatVmOpcodeType_EndConditionalBlock = 2,
        CheatVmOpcodeType_ControlLoop = 3,
        CheatVmOpcodeType_LoadRegisterStatic = 4,
        CheatVmOpcodeType_LoadRegisterMemory = 5,
        CheatVmOpcodeType_StoreStaticToAddress = 6,
        CheatVmOpcodeType_PerformArithmeticStatic = 7,
        CheatVmOpcodeType_BeginKeypressConditionalBlock = 8,

        /* These are not implemented by Gateway's VM. */
        CheatVmOpcodeType_PerformArithmeticRegister = 9,
        CheatVmOpcodeType_StoreRegisterToAddress = 10,
        CheatVmOpcodeType_Reserved11 = 11,

        /* This is a meta entry, and not a real opcode. */
        /* This is to facilitate multi-nybble instruction decoding. */
        CheatVmOpcodeType_ExtendedWidth = 12,

        /* Extended width opcodes. */
        CheatVmOpcodeType_BeginRegisterConditionalBlock = 0xC0,
        CheatVmOpcodeType_SaveRestoreRegister = 0xC1,
        CheatVmOpcodeType_SaveRestoreRegisterMask = 0xC2,

        /* This is a meta entry, and not a real opcode. */
        /* This is to facilitate multi-nybble instruction decoding. */
        CheatVmOpcodeType_DoubleExtendedWidth = 0xF0,

        /* Double-extended width opcodes. */
        CheatVmOpcodeType_DebugLog = 0xFFF,
    };

    enum MemoryAccessType : u32 {
        MemoryAccessType_MainNso = 0,
        MemoryAccessType_Heap = 1,
    };

    enum ConditionalComparisonType : u32 {
        ConditionalComparisonType_GT = 1,
        ConditionalComparisonType_GE = 2,
        ConditionalComparisonType_LT = 3,
        ConditionalComparisonType_LE = 4,
        ConditionalComparisonType_EQ = 5,
        ConditionalComparisonType_NE = 6,
    };

    enum RegisterArithmeticType : u32 {
        RegisterArithmeticType_Addition = 0,
        RegisterArithmeticType_Subtraction = 1,
        RegisterArithmeticType_Multiplication = 2,
        RegisterArithmeticType_LeftShift = 3,
        RegisterArithmeticType_RightShift = 4,

        /* These are not supported by Gateway's VM. */
        RegisterArithmeticType_LogicalAnd = 5,
        RegisterArithmeticType_LogicalOr = 6,
        RegisterArithmeticType_LogicalNot = 7,
        RegisterArithmeticType_LogicalXor = 8,

        RegisterArithmeticType_None = 9,
    };

    enum StoreRegisterOffsetType : u32 {
        StoreRegisterOffsetType_None = 0,
        StoreRegisterOffsetType_Reg = 1,
        StoreRegisterOffsetType_Imm = 2,
        StoreRegisterOffsetType_MemReg = 3,
        StoreRegisterOffsetType_MemImm = 4,
        StoreRegisterOffsetType_MemImmReg = 5,
    };

    enum CompareRegisterValueType : u32 {
        CompareRegisterValueType_MemoryRelAddr = 0,
        CompareRegisterValueType_MemoryOfsReg = 1,
        CompareRegisterValueType_RegisterRelAddr = 2,
        CompareRegisterValueType_RegisterOfsReg = 3,
        CompareRegisterValueType_StaticValue = 4,
        CompareRegisterValueType_OtherRegister = 5,
    };

    enum SaveRestoreRegisterOpType : u32 {
        SaveRestoreRegisterOpType_Restore = 0,
        SaveRestoreRegisterOpType_Save = 1,
        SaveRestoreRegisterOpType_ClearSaved = 2,
        SaveRestoreRegisterOpType_ClearRegs = 3,
    };

    enum DebugLogValueType : u32 {
        DebugLogValueType_MemoryRelAddr = 0,
        DebugLogValueType_MemoryOfsReg = 1,
        DebugLogValueType_RegisterRelAddr = 2,
        DebugLogValueType_RegisterOfsReg = 3,
        DebugLogValueType_RegisterValue = 4,
    };

    union VmInt {
        u8  bit8;
        u16 bit16;
        u32 bit32;
        u64 bit64;
    };

    struct StoreStaticOpcode {
        u32 bit_width;
        MemoryAccessType mem_type;
        u32 offset_register;
        u64 rel_address;
        VmInt value;
    };

    struct BeginConditionalOpcode {
        u32 bit_width;
        MemoryAccessType mem_type;
        ConditionalComparisonType cond_type;
        u64 rel_address;
        VmInt value;
    };

    struct EndConditionalOpcode {};

    struct ControlLoopOpcode {
        bool start_loop;
        u32 reg_index;
        u32 num_iters;
    };

    struct LoadRegisterStaticOpcode {
        u32 reg_index;
        u64 value;
    };

    struct LoadRegisterMemoryOpcode {
        u32 bit_width;
        MemoryAccessType mem_type;
        u32 reg_index;
        bool load_from_reg;
        u64 rel_address;
    };

    struct StoreStaticToAddressOpcode {
        u32 bit_width;
        u32 reg_index;
        bool increment_reg;
        bool add_offset_reg;
        u32 offset_reg_index;
        u64 value;
    };

    struct PerformArithmeticStaticOpcode {
        u32 bit_width;
        u32 reg_index;
        RegisterArithmeticType math_type;
        u32 value;
    };

    struct BeginKeypressConditionalOpcode {
        u32 key_mask;
    };

    struct PerformArithmeticRegisterOpcode {
        u32 bit_width;
        RegisterArithmeticType math_type;
        u32 dst_reg_index;
        u32 src_reg_1_index;
        u32 src_reg_2_index;
        bool has_immediate;
        VmInt value;
    };

    struct StoreRegisterToAddressOpcode {
        u32 bit_width;
        u32 str_reg_index;
        u32 addr_reg_index;
        bool increment_reg;
        StoreRegisterOffsetType ofs_type;
        MemoryAccessType mem_type;
        u32 ofs_reg_index;
        u64 rel_address;
    };

    struct BeginRegisterConditionalOpcode {
        u32 bit_width;
        ConditionalComparisonType cond_type;
        u32 val_reg_index;
        CompareRegisterValueType comp_type;
        MemoryAccessType mem_type;
        u32 addr_reg_index;
        u32 other_reg_index;
        u32 ofs_reg_index;
        u64 rel_address;
        VmInt value;
    };

    struct SaveRestoreRegisterOpcode {
        u32 dst_index;
        u32 src_index;
        SaveRestoreRegisterOpType op_type;
    };

    struct SaveRestoreRegisterMaskOpcode {
        SaveRestoreRegisterOpType op_type;
        bool should_operate[0x10];
    };

    struct DebugLogOpcode {
        u32 bit_width;
        u32 log_id;
        DebugLogValueType val_type;
        MemoryAccessType mem_type;
        u32 addr_reg_index;
        u32 val_reg_index;
        u32 ofs_reg_index;
        u64 rel_address;
    };

    struct CheatVmOpcode {
        CheatVmOpcodeType opcode;
        bool begin_conditional_block;
        union {
            StoreStaticOpcode store_static;
            BeginConditionalOpcode begin_cond;
            EndConditionalOpcode end_cond;
            ControlLoopOpcode ctrl_loop;
            LoadRegisterStaticOpcode ldr_static;
            LoadRegisterMemoryOpcode ldr_memory;
            StoreStaticToAddressOpcode str_static;
            PerformArithmeticStaticOpcode perform_math_static;
            BeginKeypressConditionalOpcode begin_keypress_cond;
            PerformArithmeticRegisterOpcode perform_math_reg;
            StoreRegisterToAddressOpcode str_register;
            BeginRegisterConditionalOpcode begin_reg_cond;
            SaveRestoreRegisterOpcode save_restore_reg;
            SaveRestoreRegisterMaskOpcode save_restore_regmask;
            DebugLogOpcode debug_log;
        };
    };

    class CheatVirtualMachine {
        public:
            constexpr static size_t MaximumProgramOpcodeCount = 0x400;
            constexpr static size_t NumRegisters = 0x10;
        private:
            size_t num_opcodes = 0;
            size_t instruction_ptr = 0;
            size_t condition_depth = 0;
            bool decode_success = false;
            u32 program[MaximumProgramOpcodeCount] = {0};
            u64 registers[NumRegisters] = {0};
            u64 saved_values[NumRegisters] = {0};
            size_t loop_tops[NumRegisters] = {0};
        private:
            bool DecodeNextOpcode(CheatVmOpcode *out);
            void SkipConditionalBlock();
            void ResetState();

            /* For implementing the DebugLog opcode. */
            void DebugLog(u32 log_id, u64 value);

            /* For debugging. These will be IFDEF'd out normally. */
            void OpenDebugLogFile();
            void CloseDebugLogFile();
            void LogToDebugFile(const char *format, ...);
            void LogOpcode(const CheatVmOpcode *opcode);

            static u64 GetVmInt(VmInt value, u32 bit_width);
            static u64 GetCheatProcessAddress(const CheatProcessMetadata* metadata, MemoryAccessType mem_type, u64 rel_address);
        public:
            CheatVirtualMachine() { }

            size_t GetProgramSize() {
                return this->num_opcodes;
            }

            bool LoadProgram(const CheatEntry *cheats, size_t num_cheats);
            void Execute(const CheatProcessMetadata *metadata);
    #ifdef DMNT_CHEAT_VM_DEBUG_LOG
        private:
            FILE *debug_log_file = NULL;
    #endif
    };

}
