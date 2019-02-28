#pragma once

namespace ddk::callback {
	//��DriverUnload�ֶ�������������
	class CProcessCallback
	{
	private://����ָ��
		using pfnPsSetCreateProcessNotifyRoutineEx = NTSTATUS(NTAPI*)(
			_In_ PCREATE_PROCESS_NOTIFY_ROUTINE_EX NotifyRoutine,
			_In_ BOOLEAN                           Remove);
		using pfnProcessCallback   = std::function<VOID(HANDLE, HANDLE, BOOLEAN)>;
		using pfnProcessCallbackEx = std::function<VOID(PEPROCESS, HANDLE, PPS_CREATE_NOTIFY_INFO)>;
	private://��Ա����
		BOOL m_bIsCallbackEx;
		pfnProcessCallback   m_callback;
		pfnProcessCallbackEx m_callback_ex;
	private://���ص����ķ���
		CProcessCallback() {
			using namespace ddk::util;
			PVOID funcAddr = CommonUtil::GetRoutineAddr(L"PsSetCreateProcessNotifyRoutineEx");
			if (funcAddr)
			{
				m_bIsCallbackEx = TRUE;
				pfnPsSetCreateProcessNotifyRoutineEx fun = (pfnPsSetCreateProcessNotifyRoutineEx)funcAddr;
				fun(ProcessCallbackEx, FALSE);
			}
			else
			{
				m_bIsCallbackEx = FALSE;
				PsSetCreateProcessNotifyRoutine(ProcessCallback, FALSE);
			}
		}
		CProcessCallback(CProcessCallback &) = delete;
		CProcessCallback& operator = (CProcessCallback) = delete;
	public://���������ķ���
		~CProcessCallback() {
			if (m_bIsCallbackEx)
			{
				using namespace ddk::util;
				PVOID funcAddr = CommonUtil::GetRoutineAddr(L"PsSetCreateProcessNotifyRoutineEx");
				pfnPsSetCreateProcessNotifyRoutineEx fun = (pfnPsSetCreateProcessNotifyRoutineEx)funcAddr;
				fun(ProcessCallbackEx, TRUE);
			}
			else 
				PsSetCreateProcessNotifyRoutine(ProcessCallback, TRUE);
			
		}
		static CProcessCallback &getIntanceRef() {
			static CProcessCallback self;
			return self;
		}
		static CProcessCallback *getInstancePtr() {
			return &getIntanceRef();
		}
	public://ע��ص�
		void RegCallback(pfnProcessCallback callback) {
			m_callback = callback;
		}
		void RegCallbackEx(pfnProcessCallbackEx callbackex)
		{
			m_callback_ex = callbackex;
		}
	private://��ʵ�ص�
		static VOID ProcessCallback(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
		{
			CProcessCallback::getIntanceRef().m_callback(ParentId, ProcessId, Create);
		}
		static VOID ProcessCallbackEx(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo) {
			CProcessCallback::getIntanceRef().m_callback_ex(Process, ProcessId, CreateInfo);
		}
	};
}