#pragma once

namespace ddk {
	//����Լ���������������������·����guid��ɧ
	//������
	class CDriver {
	private://��Ա
		lock::CFGLock m_lock;
		std::vector<PDRIVER_OBJECT> m_driverList;
		CDriver() {}
	public://����
		static CDriver& getInstance() {
			static CDriver self;
			return self;
		}
		~CDriver() {
			if (!m_driverList.empty())
				delAllDriverObjs();
		}
	public:
		//����һ���µ��������󣬲����ػ����������
		PDRIVER_OBJECT createDriverObj() {
			NTSTATUS status;
			PDRIVER_OBJECT retObj = NULL;
			DECLARE_UNICODE_STRING_SIZE(drvName, MAX_PATH);
			DECLARE_UNICODE_STRING_SIZE(ucGuid, MAX_PATH);
			//����ǰ׺
			RtlAppendUnicodeToString(&drvName, L"\\Driver\\");
			m_lock.acquire();
			do
			{
				status = ddk::util::GuidUtil::GetGUID(&ucGuid);
				if (!NT_SUCCESS(status))
					break;
				status = RtlAppendUnicodeStringToString(&drvName, &ucGuid);
				if (!NT_SUCCESS(status))
					break;
				status = IoCreateDriver(&drvName, (PDRIVER_INITIALIZE)ddk::CDriver::DrvEntry);
				if (!NT_SUCCESS(status))
					break;

				if (!m_driverList.empty())
					retObj = m_driverList.back();//��DrivEntry�Ѿ�add
			} while (FALSE);
			m_lock.release();
			return retObj;
		}
		//��winobjû����ɾ���������Զ���ʧ����������
		void delDriverObj(PDRIVER_OBJECT pDriverObject)
		{
			m_lock.acquire();
			if (pDriverObject->DriverUnload)
			{
				pDriverObject->DriverUnload(pDriverObject);
			}

			IoDeleteDriver(pDriverObject);
			for (auto it = m_driverList.begin(); it != m_driverList.end();)
			{
				if (*it == pDriverObject)
				{
					m_driverList.erase(it);
					break;
				}
				else
					it++;
			}
			//ObMakeTemporaryObject(drv_obj); ������ɾ��ʱ����һ�����ʱ�ը
			m_lock.release();
		}
	public://��̬���������߰�
		static BOOL getDriverObjectByDriverName(WCHAR *drvName, PDRIVER_OBJECT *pObj) {
			//������--->��������
			UNICODE_STRING usDrvName;
			NTSTATUS status;

			RtlInitUnicodeString(&usDrvName, drvName);
			status = ObReferenceObjectByName(&usDrvName, OBJ_CASE_INSENSITIVE, NULL, 0,
				*IoDriverObjectType, KernelMode, NULL, (PVOID *)pObj);
			if (!NT_SUCCESS(status))
			{
				ddk::log::StatusInfo::Print(status);
				return FALSE;
			}
			return TRUE;
		}
		static BOOL getDriverAllDevice(PDRIVER_OBJECT pDrvObj, std::vector<PDEVICE_OBJECT> &pDevList)
		{
			//ö��driver_object��device_object
			ULONG ArrayLength = 0;
			PDEVICE_OBJECT *DeviceArray = nullptr;
			NTSTATUS status = STATUS_SUCCESS;
			auto exit_free = std::experimental::make_scope_exit(
				[&]() {
				if (DeviceArray)
				{
					free(DeviceArray);
				}
			});

			do
			{
				status = IoEnumerateDeviceObjectList(pDrvObj, DeviceArray, ArrayLength * sizeof(PDEVICE_OBJECT), &ArrayLength);
				if (status == STATUS_BUFFER_TOO_SMALL) {
					if (DeviceArray != nullptr)
						free(DeviceArray);
					DeviceArray = nullptr;
					DeviceArray = reinterpret_cast<PDEVICE_OBJECT *>(malloc(ArrayLength * sizeof(PDEVICE_OBJECT)));
					if (!DeviceArray)
						status = STATUS_INSUFFICIENT_RESOURCES;
				}
			} while (status == STATUS_BUFFER_TOO_SMALL);

			if (NT_SUCCESS(status))
			{
				for (auto i = 0UL; i < ArrayLength; i++)
					pDevList.push_back(DeviceArray[i]);
				return TRUE;
			}
			return FALSE;
		}
		static BOOL getAllDrivers(std::vector<PDRIVER_OBJECT> &lists) {
			if (!getDriverObjectList(L"\\Driver", lists))
				return FALSE;
			if (!getDriverObjectList(L"\\FileSystem", lists))
				return FALSE;
			return TRUE;
		}
	private://˽�з���
		static NTSTATUS DrvEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING registryPath) {
			UNREFERENCED_PARAMETER(registryPath);
			//��static�����в��ܷ��ʷ�static�������õ������Խ���������
			getInstance().addDriverObj(pDriverObject);
			return STATUS_SUCCESS;
		}
		//��winobjû����ɾ���������Զ���ʧ����������
		void delAllDriverObjs() {
			for (auto elem : m_driverList)
				IoDeleteDriver(elem);
			m_driverList.clear();
		}
		void addDriverObj(PDRIVER_OBJECT pDriverObject) {
			m_driverList.push_back(pDriverObject);
			pDriverObject->DriverUnload = nullptr;
		}
		static bool getDriverObjectList(WCHAR *DrvName, std::vector<PDRIVER_OBJECT> &lists)
		{
			UNICODE_STRING nsDirName;
			RtlInitUnicodeString(&nsDirName, DrvName);
			OBJECT_ATTRIBUTES oa = {};
			InitializeObjectAttributes(&oa,
				&nsDirName,
				OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
				nullptr,
				nullptr);
			HANDLE DirectoryHandle = nullptr;
			auto ns = ZwOpenDirectoryObject(&DirectoryHandle, DIRECTORY_QUERY, &oa);
			if (NT_SUCCESS(ns))
			{
				ULONG QueryContext = 0;
				UNICODE_STRING DriverTypeStr;
				RtlInitUnicodeString(&DriverTypeStr, L"Driver");
				do
				{
					UCHAR Buffer[1024] = { 0 };
					POBJECT_DIRECTORY_INFORMATION DirInfo = (POBJECT_DIRECTORY_INFORMATION)Buffer;
					ns = ZwQueryDirectoryObject(
						DirectoryHandle,
						DirInfo,
						sizeof(Buffer),
						TRUE,
						FALSE,
						&QueryContext,
						NULL);
					if (NT_SUCCESS(ns))
					{
						if (RtlCompareUnicodeString(&DirInfo->TypeName, &DriverTypeStr, TRUE) == 0)
						{
							UNICODE_STRING FullDriverName;
							wchar_t wcsfullname[MAX_PATH] = { 0 };
							RtlInitEmptyUnicodeString(&FullDriverName, wcsfullname, sizeof(wcsfullname));
							RtlCopyUnicodeString(&FullDriverName, &nsDirName);
							RtlAppendUnicodeToString(&FullDriverName, L"\\");
							RtlAppendUnicodeStringToString(&FullDriverName, &DirInfo->Name);
							{
								PDRIVER_OBJECT DriverPtr = NULL;
								auto Status = ObReferenceObjectByName(&FullDriverName, OBJ_CASE_INSENSITIVE, NULL, GENERIC_READ, *IoDriverObjectType, KernelMode, NULL, (PVOID *)&DriverPtr);
								if (NT_SUCCESS(Status))
								{
									ObDereferenceObject(DriverPtr);
									lists.push_back(DriverPtr);
								}
							}
						}
					}
				} while (NT_SUCCESS(ns));
				ZwClose(DirectoryHandle);
				return TRUE;
			}
			return FALSE;
		}
	};
}

