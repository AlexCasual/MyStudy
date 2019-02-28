#pragma once
namespace ddk
{
	namespace callback {
		enum CPnpCallbackType
		{
			DISK = 1,
			CDROM,
			VOLUME,
			PARTITION,
			ALL,
		};
		class CPnpCallback {
		private:
			using pfnPnpCallback = std::function<NTSTATUS(PVOID, PVOID)>;
			pfnPnpCallback m_callback;
			CPnpCallbackType m_type;
			PVOID m_NotificationEntry;		//����Callbackʱ���أ�ע��ʱҪ�õ�

		public://���캯������������
			CPnpCallback(CPnpCallbackType type) : CPnpCallback()
			{
				CreateCallback(type);
			}
			~CPnpCallback() {
				if (m_NotificationEntry)
				{
					m_callback = NULL;
					IoUnregisterPlugPlayNotification(m_NotificationEntry);
				}
			}
			void RegCallback(pfnPnpCallback callback)
			{
				//���ûص������������ص�����
				if (!m_callback)
				{
					m_callback = callback;
				}
			}
		private://˽�з���
			CPnpCallback() {
				m_callback = NULL;
				m_NotificationEntry = NULL;
			}
			BOOL CreateCallback(CPnpCallbackType type)
			{
				NTSTATUS status = STATUS_UNSUCCESSFUL;
				this->m_type = type;
				switch (m_type)
				{
				case VOLUME:		//��ص�
				{
					status = IoRegisterPlugPlayNotification(
						EventCategoryDeviceInterfaceChange,
						PNPNOTIFY_DEVICE_INTERFACE_INCLUDE_EXISTING_INTERFACES,//0 Magic
						(PVOID)(&GUID_DEVINTERFACE_VOLUME),
						g_pDriverObject,
						DriverDevInterxNotifyCallBack,
						this,
						&m_NotificationEntry);
					break;
				}
				case PARTITION:		//�����ص�
					status = IoRegisterPlugPlayNotification(
						EventCategoryDeviceInterfaceChange,
						PNPNOTIFY_DEVICE_INTERFACE_INCLUDE_EXISTING_INTERFACES,//0 Magic
						(PVOID)(&GUID_DEVINTERFACE_PARTITION),
						g_pDriverObject,
						DriverDevInterxNotifyCallBack,
						this,
						&m_NotificationEntry);
					break;
				case DISK:		//Ӳ�̻ص�
					status = IoRegisterPlugPlayNotification(
						EventCategoryDeviceInterfaceChange,
						PNPNOTIFY_DEVICE_INTERFACE_INCLUDE_EXISTING_INTERFACES,//0 Magic
						(PVOID)(&GUID_DEVINTERFACE_DISK),
						g_pDriverObject,
						DriverDevInterxNotifyCallBack,
						this,
						&m_NotificationEntry);
					break;
				case CDROM:		//CDROM�ص�
					status = IoRegisterPlugPlayNotification(
						EventCategoryDeviceInterfaceChange,
						PNPNOTIFY_DEVICE_INTERFACE_INCLUDE_EXISTING_INTERFACES,
						(PVOID)(&GUID_DEVINTERFACE_CDROM),
						g_pDriverObject,
						DriverDevInterxNotifyCallBack,
						this,
						&m_NotificationEntry);
					break;
				case ALL:
					status = IoRegisterPlugPlayNotification(
						EventCategoryHardwareProfileChange,
						PNPNOTIFY_DEVICE_INTERFACE_INCLUDE_EXISTING_INTERFACES,//0 Magic
						nullptr,
						g_pDriverObject,
						DriverDevInterxNotifyCallBack,
						this,
						&m_NotificationEntry);
					break;
				default:
					break;
				}
				if (!NT_SUCCESS(status))
				{
					return FALSE;
				}
				return TRUE;
			}
		private://IoRegisterPlugPlayNotification�Ļص�
			static NTSTATUS NTAPI DriverDevInterxNotifyCallBack(PVOID NotificationStructure, PVOID Context)
			{
				KdBreakPoint();
				PDEVICE_INTERFACE_CHANGE_NOTIFICATION pNotify
					= reinterpret_cast<PDEVICE_INTERFACE_CHANGE_NOTIFICATION>(NotificationStructure);

				auto pThis = reinterpret_cast<CPnpCallback*>(Context);
				if (pThis->m_callback)
				{
					return pThis->m_callback(NotificationStructure, Context);
				}
				return STATUS_SUCCESS;
			}
		};
	}
};