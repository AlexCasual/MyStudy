#pragma once
//�豸������
#define DEVICE_NAME L"\\Device\\TestDriverB"
#define LINK_NAME   L"\\Dosdevices\\TestDriverB"

//������DriverEntry.cpp��
extern wchar_t g_RegistryPath[MAX_PATH];		//����ע���λ��
extern PDRIVER_OBJECT g_pDriverObject;			//��������
extern PVOID g_pDrvImageBase;					//����ģ�����ڴ�Ļ���ַ
extern SIZE_T g_DrvImageSize;					//����ģ���С
extern PLIST_ENTRY g_KernelModuleList;			//���Ա���ϵͳ������ģ,,��ǰָ���Լ���PKLDR_DATA_TABLE_ENTRY��
extern RTL_OSVERSIONINFOEXW g_WindowsVersion;	//��ǰϵͳ�汾��Ϣ
extern OS_INDEX g_OsIndex;						//�汾�ſ�
extern ddk::CDevice g_dev;					//nt_deviceȫ�ֱ���


