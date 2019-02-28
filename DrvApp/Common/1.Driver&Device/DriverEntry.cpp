#include <stdafx.h>

//DriverMain��DriverUnload����Main.cpp��
extern NTSTATUS DriverMain(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath);
extern VOID DriverUnload(PDRIVER_OBJECT pDriverObject);

//ȫ����Դ
wchar_t g_RegistryPath[MAX_PATH] = { 0 };
PDRIVER_OBJECT g_pDriverObject = nullptr;
PVOID g_pDrvImageBase = nullptr;
SIZE_T g_DrvImageSize = 0;
PLIST_ENTRY g_KernelModuleList = nullptr;
RTL_OSVERSIONINFOEXW g_WindowsVersion = {};
OS_INDEX g_OsIndex = OsIndex_UNK;
ddk::CDevice g_dev;

//����Unload�ĵط�
VOID Unload(PDRIVER_OBJECT pDriverObject)
{
	DriverUnload(pDriverObject);
	//��˯3�벹�ϣ���Ϊ��Щ�ص����������˵�����ִ�У������ӳ����������
	ddk::util::TimeUtil::sleep(ddk::util::TimeUtil::seconds(3));
	//����ȫ�֡���̬��������
	cc_doexit(0, 0, 0);
}

EXTERN_C NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	pDriverObject->DriverUnload = Unload;
	//g_CDevice = new ddk::CDevice;

	//��ʼ���汾��Ϣ
	ddk::util::OsVerUtil::InitVersion();
	if (g_OsIndex == OsIndex_UNK)
	{
		LOG_DEBUG("unknown system version\r\n");
		return STATUS_UNSUCCESSFUL;
	}
	//����ȫ�֡���̬���캯��
	cc_init(0);
	//��ʼ���ڴ�
	//===========��δд==============
	

	if (pRegistryPath)
	{
		RtlSecureZeroMemory(g_RegistryPath, sizeof(g_RegistryPath));
		RtlStringCchPrintfW(g_RegistryPath, RTL_NUMBER_OF(g_RegistryPath), L"%wZ", pRegistryPath);
	}

	if (pDriverObject)
	{
		g_pDriverObject = pDriverObject;
		//������仰����ʹ��ObRegisterCallbacks��ʧ�ܡ�Win10��һ�����ã��ƹ�MmVerifyCallbackFunction
		*(PULONG)((PCHAR)pDriverObject->DriverSection + 13 * sizeof(void*)) |= 0x20;
		PKLDR_DATA_TABLE_ENTRY entry = reinterpret_cast<PKLDR_DATA_TABLE_ENTRY>(pDriverObject->DriverSection);
		g_pDrvImageBase = entry->DllBase;
		g_DrvImageSize = entry->SizeOfImage;
		g_KernelModuleList = entry->InLoadOrderLinks.Flink;
	}
	
	//���ø���DriverMain
	status = DriverMain(pDriverObject, pRegistryPath);
	if (NT_SUCCESS(status))
		LOG_DEBUG("DriverImageBase= %p ImageSize=%x\r\n", g_pDrvImageBase, g_DrvImageSize);
	return status;
}



