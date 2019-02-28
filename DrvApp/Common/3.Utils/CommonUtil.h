#pragma once

namespace ddk::util {
	class CommonUtil {
	public:
		//Ȩ��
		static NTSTATUS AdjustPrivilege(IN ULONG Privilege, IN BOOLEAN  Enable)
		{
			//����Ȩ��
			NTSTATUS            status;
			HANDLE              token_handle;
			TOKEN_PRIVILEGES    token_privileges;

			status = ZwOpenProcessToken(
				NtCurrentProcess(),
				TOKEN_ALL_ACCESS,
				&token_handle
			);

			if (!NT_SUCCESS(status))
			{
				return status;
			}

			token_privileges.PrivilegeCount = 1;
			token_privileges.Privileges[0].Luid = RtlConvertUlongToLuid(Privilege);
			token_privileges.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

			status = NtAdjustPrivilegesToken(
				token_handle,
				FALSE,
				&token_privileges,
				sizeof(token_privileges),
				NULL,
				NULL
			);
			ZwClose(token_handle);
			return status;
		}
		//������ַ���
		static PVOID GetRoutineAddr(WCHAR *functionname)
		{
			UNICODE_STRING usName;	
			RtlInitUnicodeString(&usName, functionname);
			//LOG_DEBUG("%wZ\r\n", &usName);
			auto pfn = MmGetSystemRoutineAddress(&usName);
			if (pfn)
				return pfn;
			return nullptr;
		}
		//���KTHREAD��PreviousMode
		static DWORD GetPreviousModeOffset() {
			//���kthread��PreviousMode��ƫ�ƣ�32/64λ������ʹ�á�
			PVOID funAddr;
			PVOID pFoundAddr;
			DWORD retOffset = -1;
			CHAR  pattern[4] = "\x00\x00\xC3";

			do
			{
				funAddr = GetRoutineAddr(L"ExGetPreviousMode");
				if (!funAddr)
					break;
				pFoundAddr = MemUtil::MmMemMatch(funAddr, 32, pattern, sizeof(pattern) - 1);
				if (pFoundAddr)
					retOffset = *(DWORD *)((char *)pFoundAddr - 2);
			} while (FALSE);
			return retOffset ;
		}
		//����
		static SIZE_T AlignSize(SIZE_T nSize, UINT nAlign)
		{
			//nSize��С��nAlign�����ֽ�
			if (nAlign == 0)
			{
				return nSize;
			}
			
			return ((nSize + nAlign - 1) / nAlign * nAlign);
		}
		//���ϵͳ��Ϣ����ѯʲô���Լ�����
		static PVOID GetSysInfo(SYSTEM_INFORMATION_CLASS InfoClass)
		{
			NTSTATUS ns;
			ULONG RetSize, Size = 0x1100;
			PVOID Info;

			while (1)
			{
				if ((Info = malloc(Size)) == NULL)
				{
					return NULL;
				}

				RetSize = 0;
				ns = ZwQuerySystemInformation(InfoClass, Info, Size, &RetSize);
				if (ns == STATUS_INFO_LENGTH_MISMATCH)
				{
					free(Info);
					Info = NULL;

					if (RetSize > 0)
					{
						Size = RetSize + 0x1000;
					}
					else
						break;
				}
				else
					break;
			}

			if (!NT_SUCCESS(ns))
			{
				if (Info)
					free(Info);

				return NULL;
			}
			return Info;
		}
	public://UNICODE_STRING���ò���
		static NTSTATUS FGAllocateUnicodeString(_Out_ PUNICODE_STRING String) {

			PAGED_CODE();

			String->Buffer = (PWCH)ExAllocatePool(PagedPool, String->MaximumLength);

			if (String->Buffer == NULL) {
				LOG_DEBUG("[Ctx]: Failed to allocate unicode string of size 0x%x\n",
						String->MaximumLength);
				return STATUS_INSUFFICIENT_RESOURCES;
			}

			String->Length = 0;
			return STATUS_SUCCESS;
		}
		static VOID FGFreeUnicodeString(PUNICODE_STRING String)
		{
			PAGED_CODE();
			ExFreePool(String->Buffer);
			String->Length = String->MaximumLength = 0;
			String->Buffer = NULL;
		}


	};
}