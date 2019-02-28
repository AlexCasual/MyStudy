#pragma once
#include <vector>
//
//�õ��ڰ������ŵ�Ԥ����ͷ��û��
//
// Distorm v3.3 X86/X64
#include "mem_util.h"
#include "distorm/distorm.h"
#include "asmjit/asmjit/src/asmjit/base.h"
#include "Intrinsic.h"
#include "AsmGen.h"
#include "Reassembler.h"
#include "Detours.h"
#include "DetoursInternal.h"

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#ifdef _WIN64
namespace hook = Detours::X64;
#else
namespace hook = Detours::X86;
#endif
