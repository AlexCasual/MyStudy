#pragma once

namespace ddk::callback {
	//��DriverUnload�ֶ�������������
	class CThreadCallback
	{
	private:
		using pfnThreadCallback = std::function<VOID(HANDLE, HANDLE, BOOLEAN)>;
	private:
		pfnThreadCallback m_callback;
		BOOLEAN m_hasCallback = FALSE;	//�Ƿ�ע��ɹ�
	private://���ص����ķ���
		CThreadCallback() {
			NTSTATUS status = PsSetCreateThreadNotifyRoutine(ThreadCallback);
			if (NT_SUCCESS(status))
				m_hasCallback = TRUE;
		}
		CThreadCallback(CThreadCallback &) = delete;
		CThreadCallback& operator = (CThreadCallback) = delete;
	public://���������ķ���
		~CThreadCallback() {
			if (m_hasCallback)
				PsRemoveCreateThreadNotifyRoutine(ThreadCallback);
		}
		static CThreadCallback &getIntanceRef() {
			static CThreadCallback self;
			return self;
		}
		static CThreadCallback *getInstancePtr() {
			return &getIntanceRef();
		}
		void RegCallback(pfnThreadCallback callback) {
			m_callback = callback;
		}
	public:
		static VOID ThreadCallback(IN HANDLE  ProcessId, IN HANDLE  ThreadId, IN BOOLEAN  Create) {
			CThreadCallback::getIntanceRef().m_callback(ProcessId, ThreadId, Create);
		}
	};
}